# key.ps1 -Key W -Seconds 4 [-Proc NarutoStorm] -- hold a keyboard key in the game window (keybd_event).
# The PC profile reads GetAsyncKeyState, so injected keys count. Needs the window in the foreground (this brings it).
param([string]$Key='W', [double]$Seconds=3, [string]$Proc='NarutoStorm')
Add-Type @'
using System; using System.Runtime.InteropServices;
public class K { [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint fl, UIntPtr ex);
 [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
 [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int n);
 [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h); }
'@
$p = Get-Process $Proc -ErrorAction Stop | Select-Object -First 1
[K]::ShowWindow($p.MainWindowHandle, 9) | Out-Null; [K]::SetForegroundWindow($p.MainWindowHandle) | Out-Null; Start-Sleep -Milliseconds 500
$named = @{ ESC=0x1B; SPACE=0x20; ENTER=0x0D; TAB=0x09; F1=0x70; UP=0x26; DOWN=0x28; LEFT=0x25; RIGHT=0x27 }
$vk = if ($named.ContainsKey($Key.ToUpper())) { [byte]$named[$Key.ToUpper()] } else { [byte][char]$Key.ToUpper() }
[K]::keybd_event($vk, 0, 0, [UIntPtr]::Zero)
Start-Sleep -Milliseconds ([int]($Seconds * 1000))
[K]::keybd_event($vk, 0, 2, [UIntPtr]::Zero)
