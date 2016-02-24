# fetchimagefromfile
fetch image from flash memory dump file

* 该工具可以从flash的内存dump文件里提取出图片资源,可以跳过flash的资源加密.

* 方法: 可以打开浏览器或flash player先玩玩某个游戏，等看到需要的游戏资源下载的差不多了，就可以把浏览器内存dump出来,供程序提取了。

* 获取内存转储的方法:
  * win7下可以直接在进程管理器里右键程序的进程，创建转储文件得到.
  * xp下需要下载dump工具.
    
* 该工具支持的系统: 
  * linux下可以正常使用
  *  #编译:
  *  cd $YOUR_DIR/fetchimagefromfile/src/
  *  make
  *  #运行: 
  *  ./main dump_file out_dir
     
* win32下可以编过，运行有问题，没有排查过，因为我主要用linux。
