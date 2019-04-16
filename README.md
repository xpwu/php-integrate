# php-integrate

phpinte 主要实现把php代码打包为phar，支持代码的基础检查、类加载的自动生成、依赖、注解等功能。

## 一、配置要求

* 1、运行phpinte的机器的php.ini **phar.readonly**需要为false,参见[php phar](http://php.net/manual/zh/phar.configuration.php); 
* 2、运行phpinte的机器需要pcntl扩展并保证扩展能正常工作；
* 3、运行打包生成的phar文件时，不需上述两条。

## 二、基本使用

* 1、在项目的根目录新建 **project.conf.hphp** 文件，**project.conf.hphp** 所在的目录是集成系统的顶级目录；
* 2、在项目的根目录下及任何子目录下运行phpinte，phpinte会自动在本级目录及父目录查找**project.conf.hphp**文件，找到的第一个**project.conf.hphp**所在的目录即为实际的根目录。（**不要在子目录下增加project.conf.hphp文件，否则根目录会查找错误，除非是项目需要而有意这样**）
* 3、project.conf.hphp 配置相应的配置项，主要有:  
(1). $src = []; $exclude_src = []; 配置所有的源代码文件，最后的源文件为 $src diff $exclude_src 的文件，支持glob格式，并扩展了 **/ 表示递归查找，比如：**/*.inc 表示递归查找当前路径及其子路径下的所有*.inc文件，src/**/*.inc 表示递归查找src路径及其子路径下的所有*.inc文件。  
(2). $anno_and_dep_repo = ""; $annotation_processors = []; $dependencies = []; 与依赖 、注解相关的配置。$anno_and_dep_repo 配置依赖库的位置，目前仅支持子目录的依赖库设置，$dependencies 配置所有的[依赖](#dependence)库，$annotation_processors配置所有的[注解](#anno)处理器。  
(3). $build=['target' => "",'main_class' => ""]。配置集成时的target(生成的phar的名字与位置) 与 入口类，main_class 中必须有 public static function main(){} 的实现。  
* 4、phpinte -h 可以看到phpinte的帮助。

## <a name="dependence"></a>三、依赖

* 1、依赖的使用  
 1.1 使用phpinte发布的依赖。name:version 的格式  
 1.2 使用其他非phpinte发布的依赖。name/version/[*.php;class@file;path/to/index.php] 的格式。其中name与version 是加入依赖到仓库时建立的文件夹，依赖的原文件在version文件夹下  
 1.3 依赖可以放在repo指定的位置，phpinte将把repo中的依赖打包进phar中；也可以放在系统的include_path中，include_path中的依赖库不会打包进phar，所以集成环境与运行环境中都需要把三方库放到include_path中，确保集成与运行时都能找到。
* 2、依赖的发布  
需要在project.conf.hphp中配置 $publish = ['repo'=> "",'name'=> "",'version'=>""]; 然后运行phpinte -p 即可发布name:version 到 $publish["repo"]中。
* 3、依赖传递与合并。phpinte 使用最高版本的选取策略。phpinte -d 可以看到所有的依赖

## <a name="anno"></a> 四、注解

* 1、注解的使用  
使用 $annotation_processors = []即可配置所有需要使用的注解，name:version的方式表示一个注解。
* 2、注解的发布  
注解的发布与依赖很类似，但是需要在$publish['annotation_services'] = []中 填写此注解提供的所有注解服务的类，此服务类必须继承于 Inte\AbstractAnnotationServer 类。注解的编写需要依赖phpinte提供的注解核心库[核心库](#corelib).  
* 3、注解的运行  
注解process函数会把属于本工程(不包括依赖库)所有的输入类做处理，可以输出新的类文件，输出的.inc文件可能又会当成输入进入所有的注解服务的process函数中再次被注解处理，如此循环直到输入文件数为0且输出文件数为0时结束。注解不能对源文件做修改，但是可以对自己输出的文件做修改，但是修改后的文件不会再次作为输入被注解服务处理。（注：非.inc文件不会作为下次注解的输入）  
所有的依赖类都会被注解服务的processDependencyClasses函数处理一次，输出的.inc文件会与本工程的文件一样被process函数处理。  
以上输出的所有.inc文件可能(如下'注'的说明)会被最终集成进phar中(build命令时)或者发布到repo中(publish命令时)。  
**注：所有的非.inc文件或者被writer::dontInteOrPublish明确指定的.inc文件不会再次被处理器处理，也不会被集成或者发布**

## <a name="coreLib"></a>五、核心库

phpinte 提供一个核心库与一个注解核心库，在写应用代码或者注解时，可以依赖此核心库。需要下载lib文件夹下相应的库到repo文件夹中即可。

## 六、代码要求

使用phpinte集成或者发布的代码中，不能使用Inte与__Inte__作为第一命名空间，否则可能产生命名冲突。

### TODO
更好的支持_推迟到build时的注解_，目前对于是否在build时加入了此注解是注解编写者自己负责，而应该考虑是否phpinte能自动在build时探测并加载此类注解。


