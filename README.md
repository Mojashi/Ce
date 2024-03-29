# 制約プログラミング言語 Ce

SATEncodingをやりやすくするための言語です。
experimentalブランチは特殊な高速化をしています
## 導入
### 本体を持ってくる
```sh
git clone https://github.com/Mojashi/Ce.git
cd Ce
make init
make Release
```
でできると思うんですけどpythonを中で使っていてリンクがややこしいのでできないかも。ミスったらmakefileを開いて、
```
PYTHON=python3.8
```
となっている部分を普段使ってるバージョンに変えましょう。
面倒だったら
```sh
make NPRelease
```
とするとpythonに依存せずビルドできますが、その場合一部の機能（出力のフォーマッタ）が制限されます。

最終的にCe/solver、Ce/Release/ceが存在してれば良さそうです。

導入できたと思ったら
```sh
./Release/ce ./examples/sudoku.ce
```
をして実行結果を眺めましょう。

## できること
### 制約プログラミングとは
複数の変数間での制約を記述してそれを満たす変数の値を見つけるというプログラミングパラダイム（？）です。例えば、

<img src="https://latex.codecogs.com/gif.latex?$x,y,z&space;\in&space;\mathbb{Z}&space;\\&space;1&space;\le&space;xz&space;&plus;&space;y&space;\le&space;100&space;\\&space;40&space;\le&space;y&space;\le&space;60&space;\\&space;x&space;\equiv&space;0&space;(mod&space;3)&space;\\&space;z^2&space;=&space;100$"/>

みたいな制約があってこれを満たすx,y,zを求める。とかが制約プログラミング。これを記述するための言語が制約プログラミング言語です。
Ceでは、変数がTrue/Falseのみを持つ場合しか扱えないのですが、整数などは当然bool変数の組み合わせで表現できるので表現可能です。（上記の問題もとける）

examplesを見るといくつか例があるので楽しいです。
- sudoku.ce 数独のソルバ 数独のルールを記述するだけでいい、探索は勝手にやってくれる
- yajilin.ce ヤジリンのソルバ　ヤジリンのルールを記述するだけでいい、探索は勝手にやってくれる
- xorshift128.ce xorshift128の直近４個の出力から次の出力を予測するプログラム、偉い　Cで書かれたxorshift128のプログラムほぼ丸写しでいい
- bubblesort.ce バブルソートの正当性を確かめる。　Cのバブルソートのコードをほぼ丸写しでいい

### 文法とか
Ceでは大体C言語みたいな書き方をします。
先ほどの問題を8bit非負整数の範囲でCeで解くプログラムを書いてみましょう。

```c
import "stl/integer.ce"
    
Integer:(8) x,y,z;
void main(){
    toInteger(1,8) <= x*z + y;
    x*z + y <= toInteger(100,8);
    
    toInteger(40,8) <= y;
    y <= toInteger(60,8);
    
    x%toInteger(3,8) = toInteger(0,8);
    z*z = toInteger(100,8);
    
    [-x.overflow];
    [-y.overflow];
    [-z.overflow];
}
```
- １行目はIntegerが定義されているファイルを展開しています。名前の付け方が良くないのですが、Integerは符号なしです。

- ３行目、Integer:(8)ですが、Ceだとコンストラクタを型にくっつけて書けてその文脈で定義された変数全てに適用されます。Integerはコンストラクタにbit数を取りますので、x,y,zは8bit非負整数です。

- ５行目以降の式は書く制約式を表しています。toInteger(x, SIZE)は整数リテラルをIntegerに変換する関数です。C言語などだと比較演算子はbool値を返すだけですが、この場合は関係を縛る演算子として機能します。C言語的な演算子も存在しており、1 :<= x*z + y　など、演算子にコロンをつけたものはbool値として評価されます。

- １４行目以降の式はx,y,zの変数がオーバーフローしないことを述べています。

    [a,b,c,...] a,b,c...はBool型
    
という表現はCeにおいて最も基本的な式です。これは、

    a,b,c...をORでつないで評価した時の値が必ずTrueになる
    
という制約を表現します。この制約が最も基本的な式であることは、CeがSATという問題を利用していることによります。<br>
Integer型は値を表す複数のbitの他に、演算途中でオーバーフローしているかを管理するためのoverflowというBool型のメンバが存在しています。Bool型に付く単行演算子"-"はそのbool値の否定を表します。よって[-x.overflow]はxがオーバーフローしないことを制約しています。

※Integer型の演算は左右の型が同じサイズであることが前提とされています。

### 構造体の定義
構造体はいくつかの変数とそれらが満たすべき制約とそれらに対して作用する関数のセットです。

```c
struct 構造体の名前:(Type name, ...){
    メンバ変数たち
    
    prop{
        制約式たち
    }
    
    メンバ関数たち
}
```
という形で宣言されます。

    struct 構造体の名前:(Type name, ...)
    
はこの構造体の宣言と、型を利用するときに貰う引数の列挙です。
例えば、Integer構造体だと

    struct Integer:(SysInt SIZE)

となっています。SysIntはプリミティブ型で、整数リテラルが代入できます。これらの引数はメンバ変数として利用できます。


propはインスタンスが破棄されるときに実行される関数です。その構造体が持つルールや性質などを記述してください。
例えば数独を表現する構造体では数独のルールの表現がここに入ります。example/sudoku.ceなどを参照してください。

つかれたので終わりです

### 書くこと列挙
- maximize関数があります。Integerを与えるとそれを最大化してくれます。problem4.ce参考

- プリミティブ型はSynIntとBoolだけ SysIntに関しては四則演算とあまりは実装してある

- 代入は:=演算子です。=演算子は左右が等しい制約を表します。

- カッコで演算を先にやってもらう文法がまだ存在しないです。

- operatorの定義できます。integer.ceとか参考

- 結果のformatterを自分でかける。

- 色々とデバッグが足りてないのでダメです

