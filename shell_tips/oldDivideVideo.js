var WshShell;
var FileInput = "Call of Duty United Offensive_s3";
var FileFormatInput = ".mp4";
var FileFormatOutput = ".mp4";
var lastSeconds;
var cmpSeconds;
var StepSeconds;
var cmdString;

var cmpHour = 1;
var cmpMinute = 0;
var cmpSecond = 0;

var divHour = 0;
var divMinute = 10;
var divSecond = 0;

WshShell = WScript.CreateObject("WScript.Shell");

lastSeconds = 0;
cmpSeconds = (cmpHour * 3600) + (cmpMinute * 60) + cmpSecond;
StepSeconds = (divHour * 3600) + (divMinute * 60) + divSecond;

for (var iSeconds = StepSeconds; iSeconds <= cmpSeconds; iSeconds += StepSeconds) 
{
 cmdString = "ffmpeg.exe -i \"" + FileInput + FileFormatInput;
 cmdString += "\" -c copy -ss " + lastSeconds + " -to " + iSeconds;
 cmdString += " \"" + FileInput;
 cmdString += "_s";
 
 if			(iSeconds < 10)		cmdString += "0000";
 else if	(iSeconds < 100)	cmdString += "000";
 else if	(iSeconds < 1000)	cmdString += "00";
 else if	(iSeconds < 10000)	cmdString += "0";
	 
 cmdString += iSeconds;
 cmdString += FileFormatOutput + "\"";
 WshShell.Run(cmdString, 1, true);
 //WScript.Echo(cmdString);
 lastSeconds = iSeconds; 
}