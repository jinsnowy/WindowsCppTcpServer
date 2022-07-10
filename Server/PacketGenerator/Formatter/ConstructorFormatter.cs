using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PacketGenerator.Formatter
{
    public class DefaultConstructorFormatter : PacketFormatter
    {
        private string baseFormat =
@"    {0}() : 
    {1} 
    {{}}";

        private string format =
@"{0}{{}}";

        public override string Parse(PacketInfo info)
        {
            var targetProperties = getValidProperties(info.ClassType);

            var sl = new List<string>();
            foreach (var property in targetProperties)
            {
                var fieldName = NameRule.fieldName(property.Name);
                var perPacket = OnType(property.PropertyType, (parseType, types, typeStrs) => String.Format(format, fieldName));
                sl.Add(perPacket);
            }

            return String.Format(baseFormat, info.Name, String.Join(", ", sl));
        }
    }

    public class ParameterizedConstructorFormatter : PacketFormatter
    {
        private string baseFormat =
@"    {0}({1}) : 
    {2}
    {{}}";

        private string formatParameterIn =
@"const {0}& {1}In";

        private string formatIn =
@"{0}{{{0}In}}";

        public override string Parse(PacketInfo info)
        {
            var targetProperties = getValidProperties(info.ClassType);

            var sl1 = new List<string>();
            var sl2 = new List<string>();
            foreach (var property in targetProperties)
            {
                var fieldName = NameRule.fieldName(property.Name);
                var paramIn = OnType(property.PropertyType, (parseType, types, typeStrs) => String.Format(formatParameterIn, typeStrs[0], fieldName));
                var fieldInit = OnType(property.PropertyType, (parseType, types, typeStrs) => String.Format(formatIn, fieldName));

                sl1.Add(paramIn);
                sl2.Add(fieldInit);
            }

            return String.Format(baseFormat, info.Name, String.Join(", ", sl1), String.Join(", ", sl2));
        }
    }

    public class FactoryMethodFormatter : PacketFormatter
    {
        private string baseFormat =
@"    static std::shared_ptr<{0}> create({1})
    {{
        return std::make_shared<{0}>({2});
    }}";

        private string formatParameterIn =
@"const {0}& {1}In";

        private string formatIn =
@"{0}In";

        public override string Parse(PacketInfo info)
        {
            var targetProperties = getValidProperties(info.ClassType);

            var sl1 = new List<string>();
            var sl2 = new List<string>();
            foreach (var property in targetProperties)
            {
                var fieldName = NameRule.fieldName(property.Name);
                var paramIn = OnType(property.PropertyType, (parseType, types, typeStrs) => String.Format(formatParameterIn, typeStrs[0], fieldName));
                var fieldInit = OnType(property.PropertyType, (parseType, types, typeStrs) => String.Format(formatIn, fieldName));

                sl1.Add(paramIn);
                sl2.Add(fieldInit);
            }

            return String.Format(baseFormat, info.Name, String.Join(", ", sl1), String.Join(", ", sl2));
        }
    }

    public class CloneMethodFormatter : PacketFormatter
    {
        private string baseFormat =
@"    std::shared_ptr<{0}> deepCopy() {{ return create({1}); }}";

        private string formatParameterIn =
@"{0}";

        public override string Parse(PacketInfo info)
        {
            var targetProperties = getValidProperties(info.ClassType);

            var sl1 = new List<string>();
            foreach (var property in targetProperties)
            {
                var fieldName = NameRule.fieldName(property.Name);
                var paramIn = OnType(property.PropertyType, (parseType, types, typeStrs) => String.Format(formatParameterIn, fieldName));

                sl1.Add(paramIn);
            }

            return String.Format(baseFormat, info.Name, String.Join(", ", sl1));
        }
    }
}
