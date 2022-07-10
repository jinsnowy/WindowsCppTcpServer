namespace PacketGenerator
{
#pragma warning disable CS8618
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;


    public abstract class Packet
    {
    }

    public class MyPacket : Packet
    {
        public int Integer { get; set; }

        public long Long { get; set; }

        public UInt16 UInt16 { get; set; }

        public UInt32 UInt32 { get; set; }

        public UInt64 UInt64 { get; set; }

        public float Float { get; set; }

        public double Double { get; set; }

        public string Name { get; set; } = String.Empty;
    }

    public class MyList : Packet
    {
        public List<int> Ids { get; set; }
    }

    public class MySetString : Packet
    {
        public HashSet<string> Names { get; set; }
    }

    public class MyListString : Packet
    {
        public List<string> Names { get; set; }
    }

    public class MyMapIntString : Packet
    {
        public Dictionary<int, string> Container { get; set; }
    }

    public class MyMapIntInt : Packet
    {
        public Dictionary<int, int> Container { get; set; }
    }

    public class MyMapStringString : Packet
    {
        public Dictionary<string, string> Container { get; set; }
    }

    public class MyMapStringInt : Packet
    {
        public Dictionary<string, int> Container { get; set; }
    }
    
#pragma warning restore CS8618
}