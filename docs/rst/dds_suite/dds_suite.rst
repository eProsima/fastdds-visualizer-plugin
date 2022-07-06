.. include:: ../exports/alias.include
.. include:: ../exports/roles.include

.. _dds_suite:

###########################################
DDS Visualizer Plugin within Fast DDS Suite
###########################################

eProsima provides the eProsima |suite| Docker image for those who want a quick demonstration of Fast DDS running on an Ubuntu
platform. It can be downloaded from `eProsima's downloads page <https://eprosima.com/index.php/downloads-all>`_.

#.  This Docker image was built for Ubuntu 20.04 (Focal Fossa).

    To run this container you need Docker installed. From a terminal run

    .. code-block:: bash

        $ sudo apt install docker.io

#.  Download the compressed Docker image in ``.tar`` format from the
    `eProsima Downloads website <https://www.eprosima.com/index.php/downloads-all>`_.
    It is strongly recommended to download the image corresponding to the latest version of |suite|.

    |br|

#.  Extract the image by executing the following command:

    .. todo::

        check docker tar name

    .. code-block:: bash

        load ubuntu-fastdds-suite:<version>.tar

    where ``version`` is the downloaded version of |suite|.

    |br|

#.  Run the Docker container executing the following command:

    .. code-block:: bash

        xhost local:root
        docker \
            -it \
            --net=host \
            --privileged -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix \
            ubuntu-fastdds-suite:<version>

    After executing the previous command you should be able to enter a terminal shell within a docker with
    |fastdds| already installed and sourced, and several *eProsima* applications.


In the following `Documentation <https://fast-dds.docs.eprosima.com/en/latest/docker/docker.html>`_
you can find a detailed description of the Docker image and the features and applications available.

.. toctree::
   :maxdepth: 2

   suite_demo
