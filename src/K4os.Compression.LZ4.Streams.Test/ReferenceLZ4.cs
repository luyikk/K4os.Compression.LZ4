﻿using System;
using System.Diagnostics;

namespace K4os.Compression.LZ4.Streams.Test
{
	public class ReferenceLZ4
	{
		public static void Encode(string options, string input, string output)
		{
			var executable = Tools.FindFile(".tools/lz4.exe");

			var startup = new ProcessStartInfo {
				FileName = executable,
				Arguments = $"{options} -f \"{input}\" \"{output}\"",
				CreateNoWindow = true,
				UseShellExecute = false,
				//WindowStyle = ProcessWindowStyle.Hidden
			};
			var process = Process.Start(startup);
			if (process == null)
				throw new InvalidOperationException("Cannot start LZ4.exe");

			process.WaitForExit();
		}

		public static void Decode(string input, string output)
		{
			var executable = Tools.FindFile(".tools/lz4.exe");

			var startup = new ProcessStartInfo {
				FileName = executable,
				Arguments = $"-d -f \"{input}\" \"{output}\"",
				CreateNoWindow = true,
				UseShellExecute = false,
				//WindowStyle = ProcessWindowStyle.Hidden
			};
			var process = Process.Start(startup);
			if (process == null)
				throw new InvalidOperationException("Cannot start LZ4.exe");

			process.WaitForExit();
		}
	}
}
