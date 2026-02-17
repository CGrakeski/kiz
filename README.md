<div align="center">

<svg width="200" height="206" viewBox="0 0 676 697" xmlns="http://www.w3.org/2000/svg">
  <!-- K 字母主体 -->
  <path d="M245 198 L495 198 L245 598 Z" fill="#4EC0D9"/>
  <path d="M245 498 Q320 450, 400 498 L400 598 L245 598 Z" fill="#2A7BCC"/>

  <!-- 猫咪身体 -->
  <ellipse cx="390" cy="160" rx="120" ry="80" fill="white" stroke="#2D3035" stroke-width="4"/>
  <!-- 猫咪头部 -->
  <circle cx="300" cy="140" r="50" fill="white" stroke="#2D3035" stroke-width="4"/>
  <!-- 耳朵 -->
  <path d="M265 100 L250 70 L280 90 Z" fill="white" stroke="#2D3035" stroke-width="2"/>
  <path d="M335 100 L350 70 L320 90 Z" fill="white" stroke="#2D3035" stroke-width="2"/>
  <path d="M268 102 L258 80 L278 92 Z" fill="#FFB6C1"/>
  <path d="M332 102 L342 80 L322 92 Z" fill="#FFB6C1"/>
  <!-- 头部条纹 -->
  <path d="M280 110 L285 120" stroke="#A8B0B8" stroke-width="3" stroke-linecap="round"/>
  <path d="M290 110 L295 120" stroke="#A8B0B8" stroke-width="3" stroke-linecap="round"/>
  <path d="M310 110 L315 120" stroke="#A8B0B8" stroke-width="3" stroke-linecap="round"/>
  <!-- 眼睛 -->
  <path d="M275 135 Q280 140, 285 135" stroke="#2D3035" stroke-width="2" fill="none"/>
  <path d="M315 135 Q320 140, 325 135" stroke="#2D3035" stroke-width="2" fill="none"/>
  <!-- 鼻子和嘴 -->
  <path d="M298 150 L302 150 L300 154 Z" fill="#FFB6C1" stroke="#2D3035" stroke-width="1"/>
  <path d="M300 154 Q300 160, 295 162" stroke="#2D3035" stroke-width="1.5" fill="none"/>
  <path d="M300 154 Q300 160, 305 162" stroke="#2D3035" stroke-width="1.5" fill="none"/>
  <!-- 胡须 -->
  <path d="M270 145 L240 140" stroke="#2D3035" stroke-width="1" stroke-linecap="round"/>
  <path d="M270 150 L240 150" stroke="#2D3035" stroke-width="1" stroke-linecap="round"/>
  <path d="M270 155 L240 160" stroke="#2D3035" stroke-width="1" stroke-linecap="round"/>
  <path d="M330 145 L360 140" stroke="#2D3035" stroke-width="1" stroke-linecap="round"/>
  <path d="M330 150 L360 150" stroke="#2D3035" stroke-width="1" stroke-linecap="round"/>
  <path d="M330 155 L360 160" stroke="#2D3035" stroke-width="1" stroke-linecap="round"/>
  <!-- 前爪 -->
  <circle cx="300" cy="190" r="18" fill="white" stroke="#2D3035" stroke-width="2"/>
  <path d="M292 190 L296 190" stroke="#2D3035" stroke-width="1.5" stroke-linecap="round"/>
  <path d="M300 190 L304 190" stroke="#2D3035" stroke-width="1.5" stroke-linecap="round"/>
  <!-- 尾巴 -->
  <path d="M410 180 Q450 200, 470 180" stroke="#2D3035" stroke-width="4" fill="white"/>
  <path d="M430 190 L440 195" stroke="#A8B0B8" stroke-width="3" stroke-linecap="round"/>
  <path d="M445 195 L455 190" stroke="#A8B0B8" stroke-width="3" stroke-linecap="round"/>
  <path d="M460 185 L470 180" stroke="#A8B0B8" stroke-width="3" stroke-linecap="round"/>
  <!-- 身体条纹 -->
  <path d="M380 110 L400 120" stroke="#A8B0B8" stroke-width="4" stroke-linecap="round"/>
  <path d="M410 110 L430 120" stroke="#A8B0B8" stroke-width="4" stroke-linecap="round"/>
  <path d="M440 120 L460 130" stroke="#A8B0B8" stroke-width="4" stroke-linecap="round"/>
</svg>

<h1> kiz v0.7.0 🎉</h1>
</div>

📌 **现状: 修复bug与预发布正式版本(2026.1)...**

## 语言核心定位
kiz-lang 是一门 **面向对象（原型链模型）、强类型+动态类型(鸭子类型)** 的轻量化脚本语言

**使用C++开发**，采用「半编译半解析」架构，内置**栈式虚拟机**（VM）

内存安全，并有基于原子引用计数（atomic reference count）的对象模型。

## 核心设计亮点
- 通过对象的 `__parent__` 属性绑定上级对象，实现原型链继承
- 支持运算符重载与魔术方法
- int 类型为无限精度整数
- 小数类型为Decimal精准小数
- 字符串类型为utf-8字符串且支持f-string
- 🪄 多范式兼容：支持OOP、FP等主流编程范式
- 🔅 语法极简：关键字集高度精简
- ✅ 规范友好：中文注释+统一命名规范
- ✔️ 开发者友好：低门槛快速上手

## 📚 文档完善
- [快速开始](docs/快速开始.md)
- [kiz2026语法与特性文档](http://doc.kiz.random321.com)
- [从源代码构建kiz指南](docs/从源代码构建kiz指南.md)
- [xmake构建教程](docs/xmake构建教程.md)
- [内置对象&库速览](docs/内置对象&库速览.md)
- [使用ai写kiz程序指南](docs/使用ai写kiz程序指南.md)
- [项目结构与功能说明](docs/项目结构与功能说明.md)
- [kiz库开发指南](docs/kiz库开发指南.md)
- [提交issue/bug-report/feature-request/pull-request指南](docs/提交指南.md)

## 🔆 项目结构
- **ArgParser**: 解析控制台参数
- **REPL**: 交互式环境
- **Lexer**: 把源代码解析为token流(基于FSM)
- **Parser**: 把token流解析为抽象语法树(基于朴素递归下降)
- **IRGenerator**: 把抽象语法树解析为字节码
- **VM**: 执行字节码(栈式虚拟机集成GC)
- **Models**: 运行时对象模型系统(包含基于ARC的GC)
- **Builtins**: 内置对象/函数
- **SrcManager**: kiz代码源文件管理器
- **ErrorReporter**: 错误格式化与打印器
- **Depends**: 非业务工具类(Bigint, Decimal, U8String, HashMap, Dict)

## 📃 TODO
- 完善标准库

## 🪄 在线体验
- **官网**: [kiz.random321.com](http://kiz.random321.com)
- **在线文档**: [doc.kiz.random321.com](http://doc.kiz.random321.com)
- **在线运行代码**: [try.kiz.random321.com](http://try.kiz.random321.com)

## 🧷 下载链接
- **下载vscode插件**: [#](#)
- **下载源代码** : [#](#)
- **下载安装包(windows)**: [#](#)
- **下载安装包(linux)**: [#](#)
- **下载安装包(mac os)**: [#](#)

## 📧 联系
kiz-lang@outlook.com