# php-integrate

### php.ini配置要求:
    打包的机器php.ini中的phar.readonly需要为false, 参见
    http://www.golaravel.com/php/phar.configuration.html ;
    运行的机器不需要设置phar.readonly为false

### 运行原理:
    在项目的根目录新建integrate.conf.hphp文件, integrate.conf.hphp所在的目录是
    集成系统的顶级目录. 在顶级目录以及子目录下新建integrate.mk.hphp文件, 集成工具会
    根据integrate.conf.hphp中的配置和integrate.mk.hphp中的指示进行集成. 在任何
    子路径下运行phpinte 或者 php pinte.phar 即可. 可以把phpinte拷贝到系统路径中, 方便执行.
    集成系统会对代码进行检查, 代码中不允许定义全局变量和函数,只能定义命名空间和类名. 
    集成系统会自动生成AutoLoader, 不需要在代码前面加入 require 或者 include 之类的代码.

### phpinte 的运行
    1. phpinte 退出码0表示执行成功, 非0表示失败. 可用$? 获取
    2. 有两种模式, pcntl 模式 与 非pcntl模式. -h 参数可以查看模式, ---NOT pcntl, 表示非pcntl
    模式, ---USE pcntl, 表示pcntl模式. 
    3. 两种模式的区别: pcntl 模式不关心mk中文件的顺序, 非pcntl 模式必须把父类或者接口文件放在
    子类文件或者实现文件前面
    4. pcntl模式开启: 启用pcntl扩展, 自动就变为pcntl模式

### 基本配置
*   **integrate.conf.hphp的配置项**

    $type = "dev"; 编译类型   
    $phar_name = __DIR__."/main"; phar的输出文件名,会自动加上.phar 后缀   
    $index="xxx"; 指定入口文件,在phar包中需要入口文件是,可以利用index配置指定, 
    此配置与mk中$index配置等效,建议在新版中index加入conf文件中,mk文件的$index会逐步取消


*   **integrate.mk.hphp的配置选项**

    $sub_DIRS=[dir1, dir2]; 配置子路径   
    $common_FILES = []; 所有类型都包含的文件   
    $xxx_FILES=[]; 指定类型包含的文件, 比如integrate.conf.hphp中设置   
    $type = "dev",则$dev_FILES中包括的文件才会打包进phar中, 而$pro_FILES,
    $test_FILES等不会加入其中.   
    $index=xxxx; 指定入口文件,在phar包中需要入口文件是,可以利用index配置指定,
    建议在conf文件中指定,mk文件中的此配置在后续版本中会逐渐取消

### 高级配置
    pinte 可以加 -c 参数, 指定另外的文件作为顶层路径标示文件
    integrate.conf.hphp 中可以设置$mk_name = "xxx", 指定mk的文件名

### 集成第三方包:
    第三方包中的写法不一定与集成系统的要求相匹配, 直接使用$common_FILES = [] 或者
     xxx_FILES=[] 指定第三方会出现错误, 因此系统使用另外的配置选项.

    $common_NOT_CHECK_FILES = []; 打包所有需要的文件,但不用检查文件
    $require_once_FILES = []; 使用第三方包时,部分主文件常常需要使用include 或者 
        require 包含进调用我们写的调用文件,但是 在我们自己写的文件中不能使用include 或者
        require等函数. 此配置项就是解决此问题,
    $classLoader = []; 由于第三方文件不能生产AutoLoader, 部分第三方包提供给调用方
        的接口是类接口,也可以使用此配置项生产AutoLoader, 而不用在$require_once_FILES = [] 
        中加入文件, $classLoader = [] 配置的类是惰性加载. 配置参数为 "类名"=>"文件名"

### mk配置中的参数:
    所有配置中的路径都是相对于mk的文件路径.
    _FILES=[]中可以直接写文件名,也可以写  "src file name"=>"dest file name", 也
    可以使用匹配符 *.inc 等形式, 建议尽量少用*匹配符, 应该对每一个使用的文件都有明确指定
    是一种好的习惯


