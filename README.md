# php-integrate


## php.ini配置要求:

打包的机器php.ini中的**phar.readonly**需要为false, 参见[php phar](http://php.net/manual/zh/phar.configuration.php);  
运行的机器不需要设置**phar.readonly**为false

## 使用方式:

* 在项目的根目录新建 **integrate.conf.hphp** 文件，**integrate.conf.hphp** 所在的目录是集成系统的顶级目录。   
* 在顶级目录以及子目录下新建 **integrate.mk.hphp** 文件，集成工具会根据 **integrate.conf.hphp** 中的配置和 **integrate.mk.hphp** 中的说明进行集成。  
* 在任何子路径下运行`phpinte`或者`php pinte.phar`即可。  
集成系统会对代码进行检查，代码中不允许定义全局变量和全局函数，只能定义命名空间和类名。   
集成系统会自动生成 _AutoLoader_，不需要在代码前面加入`require`或者 `include`之类的代码。  

_**建议把`phpinte`拷贝到系统路径中，方便执行。**_

## phpinte 的运行

1. phpinte 退出码0表示执行成功，非0表示失败。可用$? 获取
2. 有两种模式，pcntl 模式 与 非pcntl模式。-h 参数可以查看模式:  
		---NOT pcntl, 表示非pcntl模式；   
		---USE pcntl, 表示pcntl模式. 
3. 两种模式的区别: pcntl 模式不关心mk中文件的顺序, 非pcntl 模式必须把父类或者接口文件放在子类文件或者实现文件前面
4. pcntl模式开启: 启用pcntl扩展, 自动就变为pcntl模式

## 基本配置

*   **integrate.conf.hphp的配置项**

1. `$type = "dev"`：编译类型   
2. `$phar_name = __DIR__."/main"`：phar的输出文件名，会自动加上.phar 后缀   
3. `$index="xxx"`：指定入口文件，在phar包中需要入口文件时，可以利用index配置指定。


*   **integrate.mk.hphp的配置选项**

1. `$sub_DIRS=[dir1, dir2]`：配置子路径   
2. `$common_FILES = []`：所有类型都包含的文件   
3. `$xxx_FILES=[]`：指定类型包含的文件，比如integrate.conf.hphp中设置`$type = "dev"`则`$dev_FILES`中包括的文件才会打包进phar中，而`$pro_FILES`、`$test_FILES`等不会加入其中。

## 高级配置

pinte 可以使用 __-c__ 参数，指定另外的文件作为顶层路径标示文件__integrate.conf.hphp__ 中可以设置`$mk_name = "xxx"`，指定mk的文件名

## 集成第三方包:

第三方包中的写法不一定与集成系统的要求相匹配，直接使用`$common_FILES = []`或者`xxx_FILES=[]`指定第三方会出现错误，因此系统使用另外的配置选项，有三种方式使用。

1. 把第三方代码打包进phar包中。  
使用`$sub_DIRS` 与`$common_NOT_CHECK_FILES`打包所有需要的文件；  
使用`$require_once_FILES = []`指定使用第三方代码库的引入文件。
2. 不打包第三方代码进phar中。  
使用`$require_once_exclude_FILES = []`，指定第三方代码库的引入文件。  
__注意：__需要在php.ini中指定正确的include_path保证能加载文件成功。在执行phar的环境(一般为实际运行环境)和集成phar的环境(一般为打包环境)都需要能正确的加载到第三方代码库，如果集成环境不能正确的加载指定的第三方代码文件，则phpinte会执行失败，如果执行环境不能正确加载，运行xxx.phar会失败。
3. 自己写中间文件通过`require_once`命令加载第三方包。  
第三方包不在phar中使用，`$common_NOT_CHECK_FILES=[]`打包自己写的中间文件，`$require_once_FILES = []`加载自己写的中间文件，在中间文件的 `require_once`中包括第三方包，一样需要保证`require_once`在打包和运行环境中都能正确的找到第三方包。  
     
**特备说明**  
 
如果第三方代码中，没有定义需要提前加载的全局变量，建议使用`$classLoader_XXX=[]`替换`$require_once_XXX = []`。`$classLoader`方式支持惰性加载，配置方式为"类名"=>"文件名"。替换规则为：`$require_once_FILES = []`替换为`$classLoader=[]`；`$require_once_exclude_FILES = []`替换为`$classLoader_exclude = []`
   

## mk配置中的参数:

所有配置中的路径都是相对于mk的文件路径。xxx_FILES=[]中可以直接写文件名，也可以写`"src file name"=>"dest file name"`，也可以使用匹配符`*.inc `等形式


