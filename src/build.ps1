$base_dir = $PSScriptRoot
$loc = "$base_dir/$($MyInvocation.MyCommand.Name)"

$need_to_build = $false
$dependencies = @("build.ps1", "build.sbt", "project/plugins.sbt") | ForEach-Object { Join-Path -Path $base_dir -ChildPath $_ } | Get-Item
$dependencies += (Get-ChildItem "$base_dir/source")

$launcher = "target/start.bat"
# Run the build if necessary.
if (-Not (Test-Path "$base_dir/$launcher")) {
    # If the launcher file doesn't exist, the build was never even run.
    $need_to_build = $true
} else {
    # If the launcher file isn't newer than all the build dependencies, the build needs to be rerun.
    foreach ($dependency in $dependencies) {
        if ($dependency.LastWriteTime -ge (ls "$base_dir/$launcher").LastWriteTime) {
            $need_to_build = $true
            break
        }
    }
}

if ($need_to_build) {
    echo "Building Djinni..."
    try {
        Push-Location
        Set-Location $base_dir
        .\support\sbt.ps1 compile start-script
    } finally {
        Pop-Location
    }
} else {
    echo "Already up to date: Djinni"
}
