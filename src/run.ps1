$base_dir = $PSScriptRoot

. $base_dir\build.ps1

& "$base_dir\target\start.bat" $args
