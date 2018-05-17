# DependencyMiner

我们使用C++实现了[DFD](https://dl.acm.org/citation.cfm?id=2794377)，并发现了如下疑似错误、笔误。

- Page-3 Algorithm1 中，$\textbf{foreach} RHS \in R \textbf{do}$一句，考察的对象应当包括在上一步被除去的unique column