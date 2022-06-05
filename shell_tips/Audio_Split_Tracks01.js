var WshShell;
var FileInput = "sHardWave";
var FileFormatInput = ".mp4";
var FileFormatOutput = ".wav";
var cmdString;

var dM = [ 02, 03, 05, 06, 09, 11, 12, 13, 16, 19, 22, 26, 28 ];
var dS = [ 22, 43, 34, 40, 19, 11, 05, 14, 52, 58, 26, 04, 23 ];

var lastSeconds = 0;
var iSeconds = 0;

WshShell = WScript.CreateObject("WScript.Shell");

for (var i = 0; i < dM.length; ++i)
{
 iSeconds = dM[i] * 60 + dS[i];
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