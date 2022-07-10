namespace PacketGenerator.Formatter
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using System.Reflection;

    public static class NameRule
    {
        public static string fieldName(string propertyName)
        {
            return $"m{propertyName}";
        }
    }

    public enum ParseType
    {
        Base,
        List,
        SortedSet,
        HashSet,
        SortedDictionary,
        Dictionary,
    }

    public abstract class PacketFormatter
    {
        public abstract string Parse(PacketInfo info);

        public string OnType(Type t, Func<ParseType, Type[], string[], string> formatter)
        {
            List<Type> types = new List<Type>() { t };
           
            if (TypeContainer.tryGetBaseTypeStr(Type.GetTypeCode(t), out var typeStr))
                return formatter(ParseType.Base, types.ToArray(), new string[] { typeStr });

            if (IsDictionaryType(t) || IsSortedDictionaryType(t))
            {
                var keyType = t.GetGenericArguments()[0];
                var valueType = t.GetGenericArguments()[1];

                if (!TypeContainer.tryGetBaseTypeStr(Type.GetTypeCode(keyType), out var keyTypeStr)
                    || !TypeContainer.tryGetBaseTypeStr(Type.GetTypeCode(valueType), out var valueTypeStr))
                    return "unknown";

                bool isDict = IsDictionaryType(t);
                var containerStr = isDict ? "std::unordered_map" : "std::map";
                types.Add(keyType); types.Add(valueType);
                var strs = new string[] { $"{containerStr}<{keyTypeStr}, {valueTypeStr}>", keyTypeStr, valueTypeStr };

                return formatter(isDict ? ParseType.Dictionary : ParseType.SortedDictionary, types.ToArray(), strs);
            }
            else if (IsHashSetType(t) || IsSortedSetType(t))
            {
                var valueType = t.GetGenericArguments()[0];

                if (!TypeContainer.tryGetBaseTypeStr(Type.GetTypeCode(valueType), out var valueTypeStr))
                    return "unknown";

                bool isHashSet = IsHashSetType(t);
                var containerStr = isHashSet ? "std::unordered_set" : "std::set";
                types.Add(valueType);
                var strs = new string[] { $"{containerStr}<{valueTypeStr}>", valueTypeStr };

                return formatter(isHashSet ? ParseType.HashSet : ParseType.SortedSet, types.ToArray(), strs);
            }
            else if (IsListType(t))
            {
                var valueType = t.GetGenericArguments()[0];

                if (!TypeContainer.tryGetBaseTypeStr(Type.GetTypeCode(valueType), out var valueTypeStr))
                    return "unknown";

                var containerStr = "std::vector";
                types.Add(valueType);
                var strs = new string[] { $"{containerStr}<{valueTypeStr}>", valueTypeStr };

                return formatter(ParseType.List, types.ToArray(), strs);
            }
               
            return "unknown";
        }

        public bool IsStringType(Type t)
        {
            return Type.GetTypeCode(t) == TypeCode.String;
        }

        public bool IsDictionaryType(Type t) 
        {
            return t.IsGenericType && t.GetGenericTypeDefinition() == typeof(Dictionary<,>);
        }

        public bool IsSortedDictionaryType(Type t)
        {
            return t.IsGenericType && t.GetGenericTypeDefinition() == typeof(SortedDictionary<,>);
        }

        public bool IsListType(Type t)
        {
            return t.IsGenericType && t.GetGenericTypeDefinition() == typeof(List<>);
        }

        public bool IsHashSetType(Type t)
        {
            return t.IsGenericType && t.GetGenericTypeDefinition() == typeof(HashSet<>);
        }

        public bool IsSortedSetType(Type t)
        {
            return t.IsGenericType && t.GetGenericTypeDefinition() == typeof(SortedSet<>);
        }

        public List<PropertyInfo> getValidProperties(Type type)
        {
            var targetProperties = type.GetProperties().Where(p =>
            {
                var getMethod = p.GetGetMethod();
                if (getMethod == null || getMethod.IsPrivate || getMethod.IsAbstract)
                    return false;

                var setMethod = p.GetSetMethod();
                if (setMethod == null || setMethod.IsPrivate || setMethod.IsAbstract)
                    return false;

                return true;
            });

            return targetProperties.ToList();
        }
    }

    public class NameFormatter : PacketFormatter
    {
        public override string Parse(PacketInfo info)
        {
            return String.Format($"class {info.Name} : public {info.ParentName}");
        }
    }
}
