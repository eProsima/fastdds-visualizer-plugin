.. include:: ../exports/alias.include
.. include:: ../exports/roles.include

.. _docker:

#########################################
PlotJuggler eProsima Edition Docker image
#########################################

eProsima distributes a Docker image of |eplotjuggler| with Ubuntu 20.04 as base image.
This image launches the |plotjuggler| application with the |ddsvisualizer| already installed and sourced.

#.  This Docker image was built for Ubuntu 20.04 (Focal Fossa).

    To run this container you need Docker installed. From a terminal run

    .. code-block:: bash

        sudo apt install docker.io

    |br|


#.  Download the compressed Docker image in ``.tar`` format from the |edownloads|.
    It is strongly recommended to download the image corresponding to the latest version of |ddsvisualizer|.

    |br|


#.  Extract the image by executing the following command:

    .. todo::

        check docker tar name

    .. code-block:: bash

        docker load -i ubuntu-plotjuggler-eprosima-edition:<version>.tar

    where ``version`` is the downloaded version of |ddsvisualizer|.

    |br|


#.  Run the Docker container executing the following command:

    .. code-block:: bash

        xhost local:root
        docker \
            --net=host \
            --privileged -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix \
            ubuntu-plotjuggler-eprosima-edition:<version>

    After executing the previous command you should be able to see the application running.
    Check the following section :ref:`user_manual_start` to learn how to start the |ddsvisualizer|.


Fast DDS Suite
==============

eProsima distributes a Docker image with several features and applications regarding dds, including the |eplotjuggler|.
Check section :ref:`dds_suite` to learn how to download and use the |suite|.
