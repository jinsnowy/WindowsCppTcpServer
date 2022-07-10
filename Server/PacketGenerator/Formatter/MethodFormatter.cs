using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PacketGenerator.Formatter
{
    public class TypeCodeMethodFormatter : PacketFormatter
    {
        private static string format = "	virtual int getTypeCode() const {{ return {0}; }};";

        public override string Parse(PacketInfo info)
        {
            return String.Format(format, info.TypeCode);
        }
    }

    public class SizeMethodFormatter : PacketFormatter
    {
        private static string baseFormat =
@"    virtual int getSize() const override
	{{
		int totalSize = {0}::getSize();
{1}
		return totalSize;
	}}
";

        private static string format =
@"		totalSize += {0};";

        public override string Parse(PacketInfo info)
        {
            var targetProperties = getValidProperties(info.ClassType);
           
            var ssb = new StringBuilder();
            foreach (var property in targetProperties)
            {
                var fieldName = NameRule.fieldName(property.Name);
                var perPacket = OnType(property.PropertyType, (parseType, types, typeStrs) =>
                {
                    var sb = new StringBuilder();
                    switch (parseType)
                    {
                        case ParseType.Base:
                            {
                                if (IsStringType(types[0]))
                                {
                                    sb.AppendLine(String.Format(format, $"sizeof(int)"))
                                      .Append(String.Format(format, $"(int){fieldName}.size()"));
                                }
                                else
                                {
                                    return String.Format(format, $"sizeof({typeStrs[0]})");
                                }
                            }break;
                        case ParseType.SortedSet:
                        case ParseType.HashSet:
                        case ParseType.List:
                            {
                                sb.AppendLine(string.Format(format, $"sizeof(int)"));
                                if (IsStringType(types[1]))
                                {
                                    sb.AppendLine($"\t\tfor (const auto& s : {fieldName})")
                                      .Append($"\t{String.Format(format, "(int)(sizeof(int) + s.size())")}");
                            
                                }
                                else
                                {
                                    sb.Append(String.Format(format, $"sizeof({typeStrs[1]}) * (int){fieldName}.size()"));
                                }
                            }
                            break;
                        case ParseType.SortedDictionary:
                        case ParseType.Dictionary:
                            {
                                sb.AppendLine(string.Format(format, $"sizeof(int)"));
                                if (IsStringType(types[1]))
                                {
                                    sb.AppendLine($"\t\tfor (const auto& p : {fieldName})");
                                    if (IsStringType(types[2]))
                                    {
                                        sb.Append($"\t{String.Format(format, "((int)p.first.size() + (int)p.second.size() + 8)")}");
                                    }
                                    else
                                    {
                                        sb.AppendLine($"\t{String.Format(format, $"(int)(sizeof(int) + p.first.size())")}")
                                          .Append(String.Format(format, $"sizeof({typeStrs[2]}) * (int){fieldName}.size()"));
                                    }
                                    return sb.ToString();
                                }
                                else
                                {
                                    sb.AppendLine(String.Format(format, $"sizeof({typeStrs[1]}) * (int){fieldName}.size()"));
                                    if (IsStringType(types[2]))
                                    {
                                        sb.AppendLine($"\t\tfor (const auto& p : {fieldName})")
                                          .Append($"\t{String.Format(format, "(int)(sizeof(int) + p.second.size())")}");
                                    }
                                    else
                                    {
                                        sb.Append(String.Format(format, $"(sizeof({typeStrs[1]}) + sizeof({typeStrs[2]})) * (int){fieldName}.size()"));
                                    }
                                }
                            }break;
                    }

                    return sb.ToString();     
                });
                ssb.AppendLine(perPacket);
            }

            return String.Format(baseFormat, info.ParentName, ssb.ToString());
        }
    }
}
