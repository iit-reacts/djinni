param (
    [switch]$clean = $false
)

$base_dir = $PSScriptRoot
$djinni_dir = "$(Split-Path -Parent $base_dir)\src"

$temp_out = "$base_dir\djinni-output-temp"

$djinnifile = "example.djinni"
$in = "$base_dir\$djinnifile"

$cpp_out = "$base_dir\generated-src\cpp"
$cs_out = "$base_dir\generated-src\cs"
$cppcli_out = "$base_dir\generated-src\cppcli"

$deps_file = "$temp_out\$djinnifile.deps"

#echo "base_dir = $base_dir"
#echo "djinni_dir = $djinni_dir"
#echo "djinnifile = $djinnifile"
#echo "in = $in"
#echo "cpp_out = $cpp_out"
#echo "cs_out = $cs_out"
#echo "cppcli_out = $cppcli_out"
#echo "deps_file = $deps_file"

if ($clean) {
    # TODO!
    echo "cleaning..."
    
    exit
}

# Build djinni
bash.exe $djinni_dir\build

# TODO check if it was already generated...

if (Test-Path $temp_out) {
    # Directory exists.
    Remove-Item -Recurse -Force $temp_out
}

#echo "D:\Dev\djinni\src\target\start.bat --cpp-out `"$temp_out/cpp`" --cpp-namespace textsort --ident-cpp-enum-type foo_bar --cpp-optional-template std::experimental::optional --cpp-optional-header `"^^^<experimental/optional^^^>`" --cs-out `"$temp_out/cs`" --cs-namespace textsort --cppcli-out `"$temp_out/cppcli`" --idl `"$in`" --list-in-files `"$deps_file.tmp`""

& "D:\Dev\djinni\src\target\start.bat" `
    --cpp-out "$temp_out/cpp" `
    --cpp-namespace textsort `
    --ident-cpp-enum-type foo_bar `
    --cpp-optional-template std::experimental::optional `
    --cpp-optional-header "^^^<experimental/optional^^^>" `
    `
    --cs-out "$temp_out/cs" `
    --cs-namespace Djinni.TextSort `
    --cppcli-out "$temp_out/cppcli" `
    `
    --idl "$in" `
    --list-in-files "$deps_file.tmp"

