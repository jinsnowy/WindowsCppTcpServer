namespace Program
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Reflection;

    using PacketGenerator;

    public class Program
    {
        static void Main()
        {
            var assembly = Assembly.GetEntryAssembly();
            if (assembly == null)
                return;

            var exePath = Path.GetDirectoryName(assembly.Location);
            var baseFolderPath = Path.Join(exePath, "\\..\\..\\..\\Generated\\");

            PacketGenerator packetGenerator = new PacketGenerator();
            packetGenerator.Generate(baseFolderPath);

        }
    }
}