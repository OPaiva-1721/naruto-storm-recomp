"""Check diagnostic-only APK contents and the native ABIs. No device required."""
from pathlib import Path
import argparse,hashlib,json,struct,zipfile

def inspect(path):
    with zipfile.ZipFile(path) as archive:
        if archive.testzip():raise ValueError('APK corrompido.')
        names=set(archive.namelist())
        forbidden={'.elf','.sprx','.self','.rap','.dkey','.hnk','.xfbin','.iso','.gcap'}
        for name in names:
            if name.startswith('assets/') or Path(name).suffix.lower() in forbidden:
                raise ValueError('Conteúdo não previsto no diagnóstico: '+name)
        expected={'arm64-v8a':183,'x86_64':62}
        libraries={name for name in names if name.startswith('lib/') and name.endswith('.so')}
        for abi,machine in expected.items():
            name=f'lib/{abi}/libnaruto_diagnostics.so'
            if name not in libraries:raise ValueError('Biblioteca ausente: '+name)
            data=archive.read(name)
            if data[:6]!=b'\x7fELF\x02\x01' or struct.unpack_from('<H',data,18)[0]!=machine:
                raise ValueError('ABI nativa incorreta: '+name)
        allowed={f'lib/{abi}/{lib}' for abi in expected for lib in ('libnaruto_diagnostics.so','libc++_shared.so')}
        if libraries-allowed:raise ValueError('Bibliotecas extras: '+str(libraries-allowed))
    with Path(path).open('rb') as stream:digest=hashlib.file_digest(stream,'sha256').hexdigest()
    return {'diagnostic_only_contents':True,'abis':list(expected),'sha256':digest,'bytes':Path(path).stat().st_size,
            'device_tested':False,'game_boot_tested':False,'rendering_tested':False}

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('apk',type=Path)
    args=parser.parse_args();print(json.dumps(inspect(args.apk),indent=2))
