# TAMAMO
![image](https://cloud.githubusercontent.com/assets/8033826/20907266/807f7b64-bb90-11e6-8142-2b3b15826c2d.png)

Take Advantage of Multi Accelerators Multi Operations

# about
We have a dream that freedom world for Heterogeneous computing.
* NOW : FPGA
* On Distributed platform : Multi Accelerators Multi Operations 
* Future : GPU,CPU,DSP etc... 

# 玉藻の前
## tamamoのイラストを募集しています。

## tamamoについて
tamamoはInterconnected-FPGAを使いこなすことをキッカケに作られた、マルチFPGA分散処理フレームワークです。
現在、高速な光ネットワーク・高速なフラッシュストレージ・高速な共有メモリ/レジスタが密結合したInterconnected-FPGAをマルチノードに展開することを実現しました。

このInterconnected-FPGAには、インターフェースや機能が多くつながります。
そのため9つのしっぽを持つ美しい九尾をモチーフに、このプロジェクトの名前を付けました。

長期的な展望として、FPGAだけにとどまらず、CPU・GPU処理も制御し、マルチアクセラレータ環境での多様な計算処理を実現します。

# 1st Target Multi FPGA(APX-7142) Cluster
![image](https://cloud.githubusercontent.com/assets/8033826/20906699/cac04086-bb8c-11e6-9805-ddbe6e5e1b36.png)

16-node APX-7142 on Skylake i7Cluster

## FPGA:APX-7142
https://www.avaldata.co.jp/products/z3_gigachannel/apx7142/apx7142.html

## 環境について
現在はAPX-7142を使い、16ノード環境でFPGAとCPUを計算処理に使っています。

![image](https://cloud.githubusercontent.com/assets/8033826/20907815/f6a89f84-bb93-11e6-857d-29496a3efbe7.png)

# How to use 
1. $ git clone 
2. buy APX-7142s
3. set up APX-7142
4. $ cd src
5. $ sh check_environment.sh
6. $ cd sample_project 
7. $ make 

You Enjoy Multi FPGA Boards for Data Center or Big data Analytics!


# award and article
* (CEATEC) 2016年度 萌芽的コンピュータシステム研究展示会 優秀若手デモ/ポスター賞
  http://www.ieice.org/iss/cpsy/jpn/kikaku/award.html

* ポスター/原稿

  http://ir.lib.uec.ac.jp/infolib/user_contents/9090000519/9090000519.pdf

  http://ir.lib.uec.ac.jp/infolib/user_contents/9090000519/9090000519-appendix.pdf
  

# follow framework's lead

https://github.com/open-mpi/ompi

http://dl.acm.org/citation.cfm?id=2159433