主机开发环境构建
======================

注：如果您是首次使用`Zephyr`进行开发，请遵照下面教程在您电脑上先安装`Zephyr`运行环境，并在`Zephyr`使用
`Zephyr`简易sample进行测试。以避免开发环境本身对zvm运行产生影响。如果使用过Zephyr的就可以直接按照下面教程
进行zvm环境配置。

开发环境配置
---------------
Zephyr-Based Virtual Machine 基于 ZephyrProject 代码库进行开发，构建和运行工具与沿用ZephyrProject原先的
west 工具，本项目已经将west.yml文件进行了配置，只需要使用west对系统进行初始化即可。
但是由于系统编译过程中将要使用到许多依赖项目，这里参考zephyrproject的说明文档[1]下载相应的依赖。
安装相关依赖（本使用说明针对新搭建的环境，读者可以根据需求进行相应的依赖安装，
为了保证开发者在安装过程中避免版本问题导致的不适配的错误，建议安装完全一致的版本）


1. 升级软件仓
^^^^^^^^^^^^^^^^^^^^^^

.. code:: shell

   sudo apt update
   sudo apt upgrade

2. 升级Kitware archive
^^^^^^^^^^^^^^^^^^^^^^

一般在 `~` 目录下执行下面初始化命令：

.. code:: shell

   wget https://apt.kitware.com/kitware-archive.sh
   sudo bash kitware-archive.sh

3. 升级相关依赖仓库
^^^^^^^^^^^^^^^^^^^^^^

.. code:: shell

   sudo apt install --no-install-recommends git cmake ninja-build gperf \
      ccache dfu-util device-tree-compiler wget \
      python3-dev python3-pip python3-setuptools python3-tk python3-wheel xz-utils file \
      make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1

4. 安装SDK工具
^^^^^^^^^^^^^^^^^^^^^^

下载并验证Zephyr SDK 捆绑包

.. code:: shell

   wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.1/zephyr-sdk-0.16.1_linux-x86_64.tar.xz
   wget -O - https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.1/sha256.sum | shasum --check --ignore-missing

如果需要，您可以更改0.16.1为其他版本；Zephyr SDK 版本页面包含所有可用的 SDK 版本。

提取 Zephyr SDK 捆绑包存档：

.. code:: shell

   cd <sdk download directory>
   tar xvf zephyr-sdk-0.16.1_linux-x86_64.tar.xz

运行 Zephyr SDK 捆绑包设置脚本：

.. code:: shell

   cd zephyr-sdk-0.16.1
   ./setup.sh

1. 安装west工具
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
(这里选择全局安装，若是想要在python-env中安装，参考[1]中资料)

.. code:: shell

   pip3 install --user -U west
   echo 'export PATH=~/.local/bin:"$PATH"' >> ~/.bashrc
   source ~/.bashrc

6. 版本信息核对（请确保安装依赖的版本与文档所示一致）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: shell

   cmake --version
   python3 --version
   dtc --version

7. 创建并初始化工作区
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

（1）创建工作区并拉取zvm仓库镜像
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: shell

   cd ~
   mkdir zvm_workspace && cd zvm_workspace
   git clone https://gitee.com/cocoeoli/zvm.git


（2）初始化工作仓
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: shell

   cd zvm
   west init -l /path-to/zvm

上面的'path-to'修改为自己的目录路径，执行完上面命令后，在'zvm_workspace'目录下将会生成.west文件夹，
其中'config'文件中存放了west的相关配置。此时可以通过执行如下命令查看'west'配置是否成功：

.. code:: shell

   west -h


也可以通过以下指令查看是否成功生成.west文件

.. code:: shell

   ls -a

显示有west信息后，即说明工作仓初始化成功，可以进行主机操作系统和客户机操作系统的开发。


`Prev>> 系统简介 <https://gitee.com/openeuler/zvm/blob/master/zvm_doc/1_System_Overview.rst>`__

`Next>> 核心模块介绍 <https://gitee.com/openeuler/zvm/blob/master/zvm_doc/3_Key_Modules.rst>`__



参考资料：
~~~~~~~~~~~~~~~~

[1] https://docs.zephyrproject.org/latest/index.html

[2] https://gitee.com/cocoeoli/arm-trusted-firmware-a
