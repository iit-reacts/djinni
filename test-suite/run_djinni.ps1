param (
    [switch]$clean = $false
)

$base_dir = $PSScriptRoot
$djinni_dir = "$(Split-Path -Parent $base_dir)\src"

$temp_out = "$base_dir\djinni-output-temp"
$in = "$base_dir\djinni\all.djinni"
$wchar_in = "$base_dir\djinni\wchar_test.djinni"

# Relative version of in and temp_out are used for Djinni call below so that
# generated lists of infiles/outfiles are not machine-dependent.  This
# is an artifact of the test suite, where we want the genereated files
# to be in git for examination.
$temp_out_relative = "djinni-output-temp"
$in_relative = "djinni\all.djinni"
$wchar_in_relative = "djinni\wchar_test.djinni"

$cpp_out = "$base_dir\generated-src\cpp"
$java_out = "$base_dir\generated-src\java\com\dropbox\djinni\test"
$jni_out = "$base_dir\generated-src\jni"
$objc_out = "$base_dir\generated-src\objc"
$cppcli_out = "$base_dir\generated-src\cppcli"
$yaml_out = "$base_dir\generated-src\yaml"

$in_file_list = "./generated-src/inFileList.txt"
$out_file_list = "./generated-src/outFileList.txt"

$java_package = "com.dropbox.djinni.test"

$gen_stamp = "$temp_out\gen.stamp"

if ($clean) {
    foreach ($dir in @("$temp_out", "$cpp_out", "$java_out", "$jni_out", "$objc_out", "$cppcli_out", "$yaml_out")) {
        if (Test-Path $dir) {
            echo "Deleting `"$dir`"..."
            rm -r $dir
        }
    }
    rm "$base_dir/generated-src/inFileList.txt"
    rm "$base_dir/generated-src/outFileList.txt"
    exit
}

# Build djinni
bash.exe $djinni_dir\build

# Run Djinni generation
if (Test-Path $temp_out) {
    rm -r $temp_out
}
try {
    Push-Location
    Set-Location $base_dir

    # Run Djinni generation
    & "$base_dir\..\src\target\start.bat" `
        --java-out "$temp_out_relative/java" `
        --java-package $java_package `
        --java-nullable-annotation "javax.annotation.CheckForNull" `
        --java-nonnull-annotation "javax.annotation.Nonnull" `
        --java-use-final-for-record false `
        --ident-java-field mFooBar `
        `
        --cpp-out "$temp_out_relative/cpp" `
        --cpp-namespace testsuite `
        --ident-cpp-enum-type foo_bar `
        --cpp-optional-template "std::experimental::optional" `
        --cpp-optional-header "\`"../../handwritten-src/cpp/optional.hpp\`"" `
        --cpp-extended-record-include-prefix "../../handwritten-src/cpp/" `
        --cpp-use-wide-strings true `
        `
        --jni-out "$temp_out_relative/jni" `
        --ident-jni-class NativeFooBar `
        --ident-jni-file NativeFooBar `
        `
        --objc-out "$temp_out_relative/objc" `
        --objcpp-out "$temp_out_relative/objc" `
        --objc-type-prefix DB `
        `
        --cs-namespace Djinni.TestSuite `
        --cppcli-out "$temp_out_relative/cppcli" `
        `
        --yaml-out "$temp_out_relative/yaml" `
        --yaml-out-file "yaml-test.yaml" `
        --yaml-prefix "test_" `
        `
        --idl "$wchar_in_relative"

    & "$base_dir\..\src\target\start.bat" `
        --java-out "$temp_out_relative/java" `
        --java-package $java_package `
        --java-nullable-annotation "javax.annotation.CheckForNull" `
        --java-nonnull-annotation "javax.annotation.Nonnull" `
        --java-use-final-for-record false `
        --java-implement-android-os-parcelable true `
        --ident-java-field mFooBar `
        `
        --cpp-out "$temp_out_relative/cpp" `
        --cpp-namespace testsuite `
        --ident-cpp-enum-type foo_bar `
        --cpp-optional-template "std::experimental::optional" `
        --cpp-optional-header "\`"../../handwritten-src/cpp/optional.hpp\`"" `
        --cpp-extended-record-include-prefix "../../handwritten-src/cpp/" `
        `
        --jni-out "$temp_out_relative/jni" `
        --ident-jni-class NativeFooBar `
        --ident-jni-file NativeFooBar `
        `
        --objc-out "$temp_out_relative/objc" `
        --objcpp-out "$temp_out_relative/objc" `
        --objc-type-prefix DB `
        `
        --cs-namespace Djinni.TestSuite `
        --cppcli-out "$temp_out_relative/cppcli" `
        `
        --list-in-files "$in_file_list" `
        --list-out-files "$out_file_list" `
        `
        --yaml-out "$temp_out_relative/yaml" `
        --yaml-out-file "yaml-test.yaml" `
        --yaml-prefix "test_" `
        `
        --idl "$in_relative" `
        --idl-include-path "djinni/vendor"

    # Make sure we can parse back our own generated YAML file
    cp "$base_dir/djinni/yaml-test.djinni" "$temp_out/yaml"
    & "$base_dir\..\src\target\start.bat" `
        --java-out "$temp_out_relative/java" `
        --java-package $java_package `
        --ident-java-field mFooBar `
        `
        --cpp-out "$temp_out_relative/cpp" `
        --ident-cpp-enum-type foo_bar `
        --cpp-optional-template "std::experimental::optional" `
        --cpp-optional-header "\`"../../handwritten-src/cpp/optional.hpp\`"" `
        `
        --jni-out "$temp_out_relative/jni" `
        --ident-jni-class NativeFooBar `
        --ident-jni-file NativeFooBar `
        `
        --objc-out "$temp_out_relative/objc" `
        --objcpp-out "$temp_out_relative/objc" `
        --objc-type-prefix DB `
        `
        --cs-namespace Djinni.TestSuite `
        --cppcli-out "$temp_out_relative/cppcli" `
        `
        --idl "$temp_out_relative/yaml/yaml-test.djinni"

        function NormalizePaths {
            $file_path = $args[0]
            (Get-Content $file_path) -replace ('\\', '/') | Out-File $file_path
        }

        NormalizePaths "$in_file_list"
        NormalizePaths "$out_file_list"
}
finally
{
    Pop-Location
}

# Copy changes from "$temp_out" to final dir.
function Mirror {
    $prefix = $args[0]
    $src = $args[1]
    $dest = $args[2]
    New-Item -ItemType directory -Path $dest -Force | Out-Null
    Robocopy.exe $src $dest /MIR /NS  /NDL /NP /NJH /NJS | %{"[$prefix] $_"}
}

Mirror "cpp" "$temp_out/cpp" "$cpp_out"
Mirror "java" "$temp_out/java" "$java_out"
Mirror "jni" "$temp_out/jni" "$jni_out"
Mirror "objc" "$temp_out/objc" "$objc_out"
Mirror "cppcli" "$temp_out/cppcli" "$cppcli_out"