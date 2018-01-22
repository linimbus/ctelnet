## CTelnet客户端自动化工具
Automated command execution scripts telnet tools

## 使用方法
| 配置项 | 参考值 | 含义 |
|---|---|---|
| [host] | 192.168.1.20| telnet服务器IP地址|
| [port] | 23 | 端口号 |
| [exit] | 100 |telnet服务器无输出信息超时等待时间，单位秒。0表示永久等待|


## 脚本语法
|   命令   |  参数  |   功能描述  |
|   ---   |  ---   |  ---       |
|  sleep  |  num   | 等待num毫秒时间 |
|   wait  | string | 等待字符串string出现之后才会执行下去  |
|  waitt  | string | 等待字符串string出现之前每秒输入t字母 |
|waitenter| string | 等待字符串string出现之前每秒输入回车  |
| ifexit  | string | 等待字符串string出现之后客户端自动推出 |
|waittimeout| num string| 等待字符串string出现，如果num毫秒没有出现，则执行下去 |
|  while  |string usrcmd| 循环执行用户的usrcmd，直到string出现 |
| for     | num usrcmd  | 执行num次用户的usrcmd命令 |
| exitsys | | 客户端直接退出 |
| print   | | 开启打印到窗口 |
| noprint | | 关闭打印到窗口 |
| ifelse  | num string cmd1 cmd2| 等待string出现，如果num毫秒内出现，则执行cmd1，否则执行cmd2 |
| usrcmd | | 用户自定义命令输入 |
|  | | 空行会自动输入回车 |
| %argx ||脚本中使用%x表示传入的启动参数，例如arg0、arg1...argx|
                              

*注意事项：所有命令必须顶行，命令与后面的参数是空格隔开的。
启动时必须配置正确的[host],[port],[exit]基本参数，执行完成之后会当前目录
生成的文件名称为 xxx.ini.result.txt 日志文件。*

## 启动参数
1. telnet.exe 192.168.1.20 23
2. telnet.exe 192.168.1.20
3. telnet.exe script.ini arg0 arg1 arg2
           
*注意：最大支持100参数，暂不支持'xx xx'字符串片段合并功能*

## 工具编译
```
git clone https://github.com/lixiangyun/ctelnet.git
cd ctelnet/CTelnet
./build.bat
```