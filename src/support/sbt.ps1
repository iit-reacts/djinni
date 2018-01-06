$base_dir = $PSScriptRoot

# Just the first two digits of the version.
$java_version = (Get-ItemProperty 'HKLM:\SOFTWARE\JavaSoft\Java Development Kit').CurrentVersion

if ($java_version -lt 1.6) {
    Write-Error "Executing \"java -version\" reports \"$java_version\"."
    Write-Error "We require at least Java 1.6."
    exit 1
}

$jvm_args = @()
if ($java_version -le 1.7) {
    $jvm_args += @("-XX:MaxPermSize=256M")  # Not relevant to JDK 1.8+
}

if ("$env:DJINNI_SBT_BOOT_PROPERTIES" -ne "") {
    $jvm_args += @("-Dsbt.boot.properties=`"$env:DJINNI_SBT_BOOT_PROPERTIES`"")
}

& java `
    "-Xms512M" "-Xmx1024M" "-Xss1M" "-XX:+CMSClassUnloadingEnabled" $jvm_args `
    "-Djava.security.manager" "-Djava.security.policy=`"$base_dir/sbt.security.policy`"" `
    "-jar" "`"$base_dir/sbt-launch.jar`"" `
    "-Dsbt.override.build.repos=true" `
    "-Dsbt.repository.config=`"$base_dir/sbt.resolvers.properties`"" `
    $args
