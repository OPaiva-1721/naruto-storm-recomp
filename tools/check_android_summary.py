"""Run pure Java diagnostic policy tests without Android or game data."""
from pathlib import Path
import shutil,subprocess,tempfile

root=Path(__file__).resolve().parents[1]
package='io/github/opaiva1721/narutodiag'
java=root/'android/app/src'
if not shutil.which('javac') or not shutil.which('java'):
    raise SystemExit('Requer JDK 17+ no PATH.')
with tempfile.TemporaryDirectory(prefix='naruto-android-test-') as output:
    subprocess.run(['javac','--release','17','-encoding','UTF-8','-d',output,
        str(java/'main/java'/package/'ProbeSummary.java'),
        str(java/'test/java'/package/'ProbeSummaryTest.java')],check=True)
    subprocess.run(['java','-ea','-cp',output,'io.github.opaiva1721.narutodiag.ProbeSummaryTest'],check=True)
