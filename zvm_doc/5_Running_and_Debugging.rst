运行和调试系统
======================


ZVM系统运行配置
----------------

QEMU 平台
^^^^^^^^^^^^^^^^^^^

qemu版本及配置：
~~~~~~~~~~~~~~~~~~
-  QEMU version: 6.2.0
-  QEMU model: board ``virt`` with cpu ``max``
-  Run command:

本项目基于的qemu软件来实现系统的启动和演示。需要注意的是，由于原生qemu仅支持一个非安全空间的串口，为方便演示，
我们给qemu添加了增加两个串口的补丁，补丁文件位于:

.. code:: shell

   ../zvm_config/qemu_platform/qemu/*.patch

使用说明参照该目录下的'README.rst'文件。qemu6.2.0仓库可以通过如下命令拉取

.. code:: shell

   git clone https://github.com/cocoeoli/qemu.git

拉取完成后，进入qemu仓库开始并对相关文件打好补丁，然后开始配置qemu。

.. code:: shell

   cd path-to/qemu
   mkdir build
   ../configure --target-list=aarch64
   make 
   make install

生成镜像文件位于'/path-to/qemu/build/aarch64-softmmu/'目录下，此时可以开始执行相关运行命令。


qemu platform 运行zvm：
~~~~~~~~~~~~~~~~~~~~~~~~

运行zvm方式有两个：

注： **脚本文件和命令中的'path-to'都需要替换为正确的路径。**

1. 通过`auto_build.sh`脚本执行：

.. code:: shell
   
   ./auto_build.sh debugserver qemu

2. 直接执行命令：

.. code:: shell

   /path-to/qemu-system-aarch64 \
      -cpu max -m 4G -nographic -machine virt,virtualization=on,gic-version=3 \
      -net none -pidfile qemu.pid -chardev stdio,id=con,mux=on \
      -serial chardev:con -mon chardev=con,mode=readline -serial pty -serial pty -smp cpus=4 \
      -device loader,file=/path-to/zephyr.bin,addr=0x48000000,force-raw=on \
      -device loader,file=/path-to/Image,addr=0x80000000,force-raw=on \
      -device loader,file=/path-to/linux-qemu-virt.dtb,addr=0x88000000 \
      -kernel /path-to/zvm_host.elf 

执行上面命令则会正常启动zvm,并打印如下shell，此时开始可以测试相应命令了。

.. code:: shell

   zvm_host:~#


Arm FVP 平台
^^^^^^^^^^^^^^^^^^^
Arm FVP 平台需要下载ARM DS2021，并进行一系列配置，具体教程后续再添加，

平台配置
~~~~~~~~~~~~~~~~

-  FVP Model: Cortex-A55x4_Cortex-A75x2(Installed with Arm DS2021)
-  paramters:
   ``-C bp.secure_memory=0 -C cache_state_modelled=0 -C pctl.startup=0.0.*.* \
   -C bp.refcounter.non_arch_start_at_default=1 -C bp.terminal_0.mode=raw \
   -C bp.terminal_1.mode=raw -C bp.terminal_2.mode=raw``

FVP加载镜像
~~~~~~~~~~~~~~~~~~

   Host: ‘zvm_host.elf’

Load zvm_host.elf to address: 0x80000000.

   guest: ‘zephyr.bin and ‘linux-system.axf’

Load zephyr.elf to address: 0xa0000000 Load linux-system.axf to address: 0xb0000000

运行
~~~~~~~

Open Arm DS2021 and click the ``run`` button。


虚拟机生命周期
--------------

ZVM中虚拟机的状态有以下几种：
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. 未运行状态（VM_STATUS_NEVER_RUN）：此状态VM未运行，只是创建了一个VM实体，等待首次调度。
2. 运行状态（VM_STATUS_RUNNING）：此状态VM正在运行，并占用处理器资源。
3. 挂起状态（VM_STATUS_PAUSE）：此状态VM暂停，相关线程在等待队列中，不占用物理CPU，随时可以调度。
4. 停止状态（VM_STATUS_STOP）：此状态VM停止，相关线程都停止执行，相关内存和I/O资源准备释放。

一个完整的生命周期包括VM的创建、VM的运行、VM的暂停和VM的退出等一系列流程。
用户通过相应的shell接口实现对VM在各种状态下的切换。


测试ZVM命令
--------------

创建虚拟机：
^^^^^^^^^^^^^^^^^^^

.. code:: c++

   zvm new -t linux    # new a linux vm with vmid that allocated by system. 

   zvm new -t zephyr   # new a zephyr vm with vmid that allocated by system. 


运行虚拟机：
^^^^^^^^^^^^^^^^^^^

.. code:: shell

   zvm run -n 0        # run the vm with vm\'s vmid equal to 0 

   zvm run -n 1        # run the vm with vm\'s vmid equal to 1 


取消虚拟机：
^^^^^^^^^^^^^^^^^^^

.. code:: shell

   zvm pause -n 0      # pause the vm with vm\'s vmid equal to 0 

   zvm pause -n 1      # pause the vm with vm\'s vmid equal to 1 


打印虚拟机信息：
^^^^^^^^^^^^^^^^^^^

.. code:: shell

   zvm info            # list vms. 


删除虚拟机：
^^^^^^^^^^^^^^^^^^^

.. code:: shell
   
   zvm delete -n 0     # delete the vm with vm\'s vmid equal to 0 

   zvm delete -n 1     # delete the vm with vm\'s vmid equal to 1 


`Prev>> ZVM系统构建 <https://gitee.com/cocoeoli/zvm/blob/master/zvm_doc/4_System_Build.rst>`

`Next>> 拓展技术介绍 <https://gitee.com/cocoeoli/zvm/blob/master/zvm_doc/6_Expansion_Technology.rst>`


问题反馈
--------
欢迎大家反馈开发中遇到的问题，下面是联系邮箱。
[1] xiongcl@hnu.edu.cn