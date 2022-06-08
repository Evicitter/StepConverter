
var WshShell;
var FileInput = "Skeler";
var FileFormatInput = ".mp4";
var FileFormatOutput = ".wav";
var lastSeconds;
var cmpSeconds;
var nextSeconds;
var cmdString;

const endHour = 0;
const endMinute = 5;
const endSecond = 0;

const divHour = 0;
const divMinute = 5;
const divSecond = 0;

WshShell = WScript.CreateObject("WScript.Shell");

lastSeconds = 0;
cmpSeconds = (endHour * 3600) + (endMinute * 60) + endSecond;
nextSeconds = (divHour * 3600) + (divMinute * 60) + divSecond;

for (var iSeconds = nextSeconds; iSeconds <= cmpSeconds; iSeconds += nextSeconds) 
{
 cmdString = "ffmpeg.exe -i \"" + FileInput + FileFormatInput + "\" -vn -ss " + lastSeconds + " -to " + iSeconds + " -ar 44100 \"" + FileInput + "_s" + iSeconds + FileFormatOutput + "\"";
 WshShell.Run(cmdString, 1, true);
 //WScript.Echo(cmdString);
 lastSeconds = iSeconds;
}