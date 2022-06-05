var WshShell;
var FileInput = "NightDriveII";
var FileFormatInput = ".mp4";
var FileFormatOutput = ".wav";
var cmdString;

var dH = [ 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01];
var dM = [ 04, 07, 10, 13, 15, 18, 21, 24, 26, 29, 31, 33, 35, 37, 40, 41, 44, 47, 49, 52, 56, 00, 04, 07, 10, 13, 17, 19, 21, 23, 25, 28];
var dS = [ 02, 41, 27, 04, 11, 24, 36, 00, 49, 33, 28, 11, 19, 55, 11, 37, 04, 24, 43, 50, 17, 33, 21, 33, 31, 16, 18, 55, 26, 23, 36, 21];

var lastSeconds = 0;
var iSeconds = 0;

WshShell = WScript.CreateObject("WScript.Shell");

for (var i = 0; i < dM.length; ++i)
{
 iSeconds = dH[i] * 3600 + dM[i] * 60 + dS[i];
 cmdString = "ffmpeg.exe -i \"" + FileInput + FileFormatInput;
 cmdString += "\" -vn -ac 2 -s 44100 -ss " + lastSeconds + " -to " + iSeconds;
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