$filenames = @()
ForEach($file in $(Get-ChildItem $args "*.cpp")) { $filenames += $file.fullname }
ForEach($filename in $filenames)
{
	$lines = Get-Content $filename
	$newLines = @("`r`n#include `"stdafx.h`"`r`n") + $lines
	Set-Content $filename $newLines
}