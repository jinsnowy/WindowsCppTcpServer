namespace PacketGenerator
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using System.Reflection;
    using Formatter;

    public class PacketInfo
    {
        public string ParentName { get; set; } = typeof(Packet).Name;
        public string Name { get; set; } = string.Empty;
        public Type ParentType { get; set; } = typeof(Packet);
        public Type ClassType { get; set; } = typeof(Packet);

        public int TypeCode { get; set; }
    }

    public class TypeInfo
    {
        public string Name { get; set; } = string.Empty;
        public int TypeCode { get; set; }
    }

    public static class TypeContainer
    {
        public static readonly List<TypeInfo> typeInfoContainer = new List<TypeInfo>();
        public static readonly Dictionary<TypeCode, TypeInfo> typeInfoMap = new Dictionary<TypeCode, TypeInfo>();

        static TypeContainer()
        {
            int startTypeCode = 0;
            void addType(TypeCode typeCode, TypeInfo info)
            {
                info.TypeCode = startTypeCode++;
                typeInfoContainer.Add(info);
                typeInfoMap.Add(typeCode, info);
            }

            addType(TypeCode.Boolean, new TypeInfo() { Name = "bool" });
            addType(TypeCode.Char, new TypeInfo() { Name = "char" });
            addType(TypeCode.Byte, new TypeInfo() { Name = "unsigned char" });
            addType(TypeCode.Int32, new TypeInfo() { Name = "int" });
            addType(TypeCode.UInt32, new TypeInfo() { Name = "unsigned int" });
            addType(TypeCode.Int16, new TypeInfo() { Name = "short" });
            addType(TypeCode.UInt16, new TypeInfo() { Name = "unsigned short" });
            addType(TypeCode.Int64, new TypeInfo() { Name = "long long" });
            addType(TypeCode.UInt64, new TypeInfo() { Name = "unsigned long long" });
            addType(TypeCode.Single, new TypeInfo() { Name = "float" });
            addType(TypeCode.Double, new TypeInfo() { Name = "double" });
            addType(TypeCode.String, new TypeInfo() { Name = "std::string" });
        }

        public static bool tryGetBaseTypeStr(TypeCode typeCode, out string typeStr)
        {
            if (typeInfoMap.ContainsKey(typeCode))
            {
                typeStr = typeInfoMap[typeCode].Name;
                return true;
            }

            typeStr = "unknown";
            return false;
        }

    }

    internal class PacketGenerator
    {
        private SortedDictionary<string, PacketInfo> packetInfoContainer = new SortedDictionary<string, PacketInfo>();
        private string BaseFolderName { get; set; } = string.Empty;

        public void Generate(string baseFolderName)
        {
            BaseFolderName = baseFolderName;
            Directory.CreateDirectory(BaseFolderName);

            var targetType = typeof(Packet);
            var types = AppDomain.CurrentDomain.GetAssemblies()
                .SelectMany(s => s.GetTypes())
                .Where(p => targetType.IsAssignableFrom(p) && !p.IsAbstract).ToList();

            foreach (var type in types)
            {
                var name = type.Name;
                var info = new PacketInfo();
                info.Name = name;
                info.ClassType = type;

                packetInfoContainer.Add(name, info);
            }

            try
            {
                GenerateFileForStruct();
                GenerateFileForType();
                GenerateFileForFactory();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception : {ex}");
            }
        }

   
        private void GenerateFileForStruct()
        {
            string structFileName = Path.Combine(BaseFolderName, GeneratedFormat.StructFileName);
            if (File.Exists(structFileName))
            {
                File.Delete(structFileName);
            }
  
            using var fs = File.Create(structFileName);
            using var writer = new StreamWriter(fs);
            var sb = new StringBuilder();

            foreach (var info in TypeContainer.typeInfoContainer)
            {
                sb.Append(String.Format(GeneratedFormat.PerPacketFormatForStruct, info.Name, info.TypeCode));
            }

            int curTypeCode = GeneratedFormat.StartTypeCode;
            foreach (var pair in packetInfoContainer)
            {
                var info = pair.Value;
                info.TypeCode = curTypeCode++;

                var text = String.Format(GeneratedFormat.PerPacketFormatForStruct, pair.Key, info.TypeCode);
                sb.Append(text);
            }

            string textForStructFile = string.Format(GeneratedFormat.BaseFormatForStruct, sb.ToString());
            writer.Write(textForStructFile);
        }

        private void GenerateFileForType()
        {
            string typeFileName = Path.Combine(BaseFolderName, GeneratedFormat.TypeFileName);
            if (File.Exists(typeFileName))
            {
                File.Delete(typeFileName);
            }

            using var fs = File.Create(typeFileName);
            using var writer = new StreamWriter(fs);

            var fmts = new List<PacketFormatter>();
            fmts.Add(new NameFormatter());
            fmts.Add(new FieldFormatter());
            fmts.Add(new DefaultConstructorFormatter());
            fmts.Add(new ParameterizedConstructorFormatter());
            fmts.Add(new GetSetMethodFormatter());
            fmts.Add(new FactoryMethodFormatter());
            fmts.Add(new CloneMethodFormatter());
            fmts.Add(new TypeCodeMethodFormatter());
            fmts.Add(new SizeMethodFormatter());
            fmts.Add(new SerializeFormatter());
            fmts.Add(new DeserializeFormatter());

            var sb = new StringBuilder();
            foreach (var pair in packetInfoContainer)
            {
                var info = pair.Value;
                var results = fmts.Select(fmt => fmt.Parse(info)).ToArray();

                string text = String.Format(GeneratedFormat.PerPacketFormatForType, results);
                sb.Append(text);
            }

            string textForTypeFile = string.Format(GeneratedFormat.BaseFormatForType, sb.ToString());
            writer.Write(textForTypeFile);
        }

        private void GenerateFileForFactory()
        {
            string factoryFileName = Path.Combine(BaseFolderName, GeneratedFormat.FactoryFileName);
            if (File.Exists(factoryFileName))
            {
                File.Delete(factoryFileName);
            }

            using var fs = File.Create(factoryFileName);
            using var writer = new StreamWriter(fs);

            StringBuilder sb = new StringBuilder();
            foreach (var pair in packetInfoContainer)
            {
                var info = pair.Value;
                var text = String.Format(GeneratedFormat.PerPacketFormatForFactory, pair.Key, info.TypeCode);
                sb.AppendLine(text);
            }

            string textForFactoryFile = string.Format(GeneratedFormat.BaseFormatForFactory, sb.ToString());
            writer.Write(textForFactoryFile);
        }
    }
}
