using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PacketGenerator.Formatter
{
    public class FieldFormatter : PacketFormatter
    {
        private static string format =
@"    {0} {1};";

        public override string Parse(PacketInfo info)
        {
            var targetProperties = getValidProperties(info.ClassType);

            var sb = new StringBuilder();
            foreach (var property in targetProperties)
            {
                var fieldName = NameRule.fieldName(property.Name);
                var perPacket = OnType(property.PropertyType, (parseType, types, typeStrs) =>
                {
                    return String.Format(format, typeStrs[0].PadRight(32, ' '), fieldName);
                });
                sb.AppendLine(perPacket);
            }

            return sb.ToString();
        }
    }

    public class GetSetMethodFormatter : PacketFormatter
    {
        private string format =
@"    const {0}& get{1}() const {{ return {2}; }} void set{1}(const {0}& {2}In) {{ {2} = {2}In; }}";

        public override string Parse(PacketInfo info)
        {
            var targetProperties = getValidProperties(info.ClassType);

            var sb = new StringBuilder();
            foreach (var property in targetProperties)
            {
                var Name = property.Name;
                var fieldName = NameRule.fieldName(property.Name);
                var perPacket = OnType(property.PropertyType, (parseType, types, typeStrs) =>
                {
                    return String.Format(format, typeStrs[0], Name, fieldName);
                });
                sb.AppendLine(perPacket);
            }

            return sb.ToString();
        }
    }
}
