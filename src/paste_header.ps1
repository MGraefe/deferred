$header = @(
"//----------------------------------",
"// Deferred Source Code",
"// Copyright (c) 2012 Marius Gräfe",
"// All rights reserved",
"// Contact: graefemarius@web.de",
"//-----------------------------------")
$filenames = @()
ForEach($file in $(Get-ChildItem *.* -include *.cpp,*.h)) 
{
    $filecontent = Get-Content $file.FullName
    $fullheader = @("//" + $file.Name) + $header
    $fullheaderStr = [string]::join([environment]::newline, $fullheader);
    $fileheaderStr = [string]::join([environment]::newline, $filecontent[0..$header.Length]);
    if(!$fullheaderStr.CompareTo($fileheaderStr) -eq 0)
    {
        Set-Content $file.FullName ($fullheader + $filecontent)
        write-host "$($file.FullName)`t pasted header" -backgroundcolor DarkGreen -foregroundcolor White;
    } else {
        write-host "$($file.FullName)`t pasted header NOT" -backgroundcolor DarkRed -foregroundcolor White;
    }
}

write-host "Press any key to continue..."
$x = $host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown") 
