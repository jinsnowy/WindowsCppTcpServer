using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PacketGenerator.Formatter
{
    internal class SerializeFormatter : PacketFormatter
    {
        private string baseFormat =
@"    virtual bool serialize(std::vector<unsigned char>& bytes, size_t& offset) override
	{{
		if (!{0}::serialize(bytes, offset))
			return false;
{1}
		return true;
	}}";

        private string format =
@"        if (!Serializer::tryWriteBytes({0}, bytes, offset))
			return false;
		offset += {1};";

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
                                sb.AppendLine(String.Format(format, $"(int){fieldName}.size()", "sizeof(int)"))
                                  .Append(String.Format(format, fieldName, $"(int){fieldName}.size()"));
                            }
                            else
                            {
                                return String.Format(format, fieldName, $"sizeof({typeStrs[0]})");
                            }
                        }break;
                        case ParseType.SortedSet:
                        case ParseType.HashSet:
                        case ParseType.List:
                        {
                            if (IsStringType(types[1]))
                            {
                                sb.AppendLine(String.Format(format, $"(int){fieldName}.size()", "sizeof(int)"))
                                    .AppendLine($"\t\tfor (const auto& s : {fieldName})")
                                    .AppendLine("\t\t{")
                                    .AppendLine(String.Format(format, $"(int)s.size()", "sizeof(int)"))
                                    .AppendLine(String.Format(format, "s", $"(int)s.size()"))
                                    .Append("}\t\t");
                            }
                            else
                            {
                                if (parseType == ParseType.List)
                                {
                                    sb.AppendLine($"\t\tint lengthOf{property.Name} = (int){fieldName}.size();")
                                    .AppendLine(String.Format(format, $"(int){fieldName}.size()", "sizeof(int)"))
                                    .Append(String.Format(format, $"(unsigned char*){fieldName}.data(), (sizeof({typeStrs[1]}) * lengthOf{property.Name})", $"(sizeof({typeStrs[1]}) * lengthOf{property.Name})"));
                                }
                                else
                                {
                                    sb.AppendLine($"\t\tfor(const auto& p : {fieldName})")
                                        .AppendLine("\t\t{")
                                        .AppendLine(String.Format(format, "p", $"sizeof({typeStrs[1]})"))
                                        .Append("\t\t}");
                                }
                            }
                        }break;
                        case ParseType.SortedDictionary:
                        case ParseType.Dictionary:
                        {
                            if (IsStringType(types[1]))
                            {
                                sb.AppendLine(String.Format(format, $"(int){fieldName}.size()", "sizeof(int)"))
                                    .AppendLine($"\t\tfor (const auto& p : {fieldName})")
                                    .AppendLine("\t\t{")
                                    .AppendLine(String.Format(format, $"(int)p.first.size()", "sizeof(int)"))
                                    .AppendLine(String.Format(format, "p.first", $"(int)p.first.size()"));
                                       
                                if (IsStringType(types[2]))
                                {
                                    sb.AppendLine(String.Format(format, $"(int)p.second.size()", "sizeof(int)"))
                                        .AppendLine(String.Format(format, "p.second", $"(int)p.second.size()"))
                                        .Append("\t\t}");
                                }
                                else
                                {
                                    sb.AppendLine(String.Format(format, "p.second", $"sizeof({typeStrs[2]})"))
                                        .Append("\t\t}");
                                }
                            }
                            else
                            {
                                sb.AppendLine(String.Format(format, $"(int){fieldName}.size()", "sizeof(int)"))
                                    .AppendLine($"\t\tfor (const auto& p : {fieldName})")
                                    .AppendLine("\t\t{")
                                    .AppendLine(String.Format(format, "p.first", $"sizeof({typeStrs[1]})"));

                                if (IsStringType(types[2]))
                                {
                                    sb.AppendLine(String.Format(format, $"(int)p.second.size()", "sizeof(int)"))
                                        .AppendLine(String.Format(format, "p.second", $"(int)p.second.size()"))
                                        .Append("\t\t}");
                                }
                                else
                                {
                                    sb.AppendLine(String.Format(format, "p.second", $"sizeof({typeStrs[2]})"))
                                        .Append("\t\t}");
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

    internal class DeserializeFormatter : PacketFormatter
    {
        private string baseFormat =
@"    virtual void deserialize(const std::vector<unsigned char>& bytes, size_t& offset) override
	{{
		{0}::deserialize(bytes, offset);
{1}    }}";

        private string format =
@"        {0} = Serializer::{1};     
		offset += {2};";

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
                                    sb.AppendLine(String.Format(format, $"int lengthOf{property.Name}", $"toValue<int>(bytes, offset)",  "sizeof(int)"))
                                      .Append(String.Format(format, fieldName, $"toString(bytes, lengthOf{property.Name}, offset)",  $"(int){fieldName}.size()"));
                                }
                                else
                                {
                                    return String.Format(format, fieldName, $"toValue<{typeStrs[0]}>(bytes, offset)",  $"sizeof({typeStrs[0]})");
                                }
                            }
                            break;
                        case ParseType.SortedSet:
                        case ParseType.HashSet:
                        case ParseType.List:
                            {
                                sb.AppendLine(String.Format(format, $"int lengthOf{property.Name}", "toValue<int>(bytes, offset)", "sizeof(int)"));
                                if (IsStringType(types[1]))
                                {
                                    sb.AppendLine($"\t\tfor (int i = 0; i < lengthOf{property.Name}; ++i)")
                                        .AppendLine("\t\t{")
                                        .AppendLine(String.Format(format, $"int length", "toValue<int>(bytes, offset)", "sizeof(int)"))
                                        .AppendLine(String.Format(format, $"std::string str", $"toString(bytes, length, offset)", $"length"));
                                    if (parseType == ParseType.List)
                                    {
                                        sb.AppendLine($"\t\t{fieldName}.emplace_back(std::move(str));");
                                    }
                                    else
                                    {
                                        sb.AppendLine($"\t\t{fieldName}.emplace(std::move(str));");
                                    }
                                    sb.Append("\t\t}");
                                }
                                else
                                {
                                    if (parseType == ParseType.List)
                                    {
                                        sb.AppendLine($"\t\t{fieldName}.resize(lengthOf{property.Name});")
                                            .AppendLine($"\t\tfor (int i = 0; i < lengthOf{property.Name}; ++i)")
                                            .AppendLine("\t\t{")
                                            .AppendLine(String.Format(format, $"{fieldName}[i]", $"toValue<{typeStrs[1]}>(bytes, offset)", $"sizeof({typeStrs[1]})"))
                                            .Append("\t\t}");
                                    }
                                    else
                                    {
                                        sb.AppendLine($"\t\tfor (int i = 0; i < lengthOf{property.Name}; ++i)")
                                            .AppendLine("\t\t{")
                                            .AppendLine(String.Format(format, $"{typeStrs[1]} value", $"toValue<{typeStrs[1]}>(bytes, offset)", $"sizeof({typeStrs[1]})"))
                                            .AppendLine($"\t\t{fieldName}.emplace(value);")
                                            .Append("\t\t}");
                                    }
                                }
                            }
                            break;
                        case ParseType.SortedDictionary:
                        case ParseType.Dictionary:
                            {
                                sb.AppendLine(String.Format(format, $"int lengthOf{property.Name}", "toValue<int>(bytes, offset)", "sizeof(int)"))
                                  .AppendLine($"\t\tfor (int i = 0; i < lengthOf{property.Name}; ++i)")
                                  .AppendLine("\t\t{");

                                if (IsStringType(types[1]))
                                {
                                    sb.AppendLine(String.Format(format, $"int lengthOfKeyStr", "toValue<int>(bytes, offset)", "sizeof(int)"))
                                      .AppendLine(String.Format(format, "std::string key", $"toString(bytes, lengthOfKeyStr, offset)", "lengthOfKeyStr"));
                                    if (IsStringType(types[2]))
                                    {
                                        sb.AppendLine(String.Format(format, $"int lengthOfValueStr", "toValue<int>(bytes, offset)", "sizeof(int)"))
                                            .AppendLine(String.Format(format, "std::string value", $"toString(bytes, lengthOfValueStr, offset)", "lengthOfValueStr"));

                                    }
                                    else
                                    {
                                        sb.AppendLine(String.Format(format, $"{typeStrs[2]} value", $"toValue<{typeStrs[2]}>(bytes, offset)", $"sizeof({typeStrs[2]})"));
                                    }

                                    var valueStr = IsStringType(types[2]) ? "std::move(value)" : "value";
                                    sb.AppendLine($"\t\t{fieldName}.emplace(std::move(key), {valueStr});")
                                      .Append("\t\t}");
                                }
                                else
                                {
                                    sb.AppendLine(String.Format(format, $"{typeStrs[1]} key", $"toValue<{typeStrs[1]}>(bytes, offset)", $"sizeof({typeStrs[1]})"));
                                    if (IsStringType(types[2]))
                                    {
                                        sb.AppendLine(String.Format(format, $"int lengthOfValueStr", "toValue<int>(bytes, offset)", "sizeof(int)"))
                                          .AppendLine(String.Format(format, "std::string value", $"toString(bytes, lengthOfValueStr, offset)", "lengthOfValueStr"));
                                    }
                                    else
                                    {
                                        sb.AppendLine(String.Format(format, $"{typeStrs[2]} value", $"toValue<{typeStrs[2]}>(bytes, offset)", $"sizeof({typeStrs[2]})"));
                                    }

                                    var valueStr = IsStringType(types[2]) ? "std::move(value)" : "value";
                                    sb.AppendLine($"\t\t{fieldName}.emplace(key, {valueStr});")
                                      .Append("\t\t}");
                                }
                            }
                            break;
                    }
                    return sb.ToString();                    
                });
                ssb.AppendLine(perPacket);
            }

            return String.Format(baseFormat, info.ParentName, ssb.ToString());
        }
    }
}
