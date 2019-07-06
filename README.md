# leptjson
learn lepton json project
## tutorial07
### 关于json的字符串生成的补充
https://docs.microsoft.com/zh-cn/sql/relational-databases/json/how-for-json-escapes-special-characters-and-control-characters-sql-server?view=sql-server-2017

如果源数据包含特殊字符，则子句在 leptjson 输出中会使用 \ 对其进行转义。

如果源数据包含控制字符，则子句在 leptjson 输出中会使用 \uxxxx格式对其进行转义。  

    CHAR(0) ---> \u0000
    CHAR(1)	---> \u0001
    .....   ---> .....
    CHAR(31)---> \u001f
