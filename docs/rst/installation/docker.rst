.. include:: ../exports/alias.include
.. include:: ../exports/roles.include

.. _docker:

##################################
DDS Visualizer Plugin Docker image
##################################

eProsima distributes a Docker image of |ddsvisualizer| with Ubuntu 20.04 as base image.
This image launches the |plotjuggler| application with the |ddsvisualizer| already installed and sourced.

#.  Download the compressed Docker image in ``.tar`` format from the
    `eProsima Downloads website <https://www.eprosima.com/index.php/downloads-all>`_.
    It is strongly recommended to download the image corresponding to the latest version of |ddsrouter|.

    |br|

#.  Extract the image by executing the following command:

    .. todo::

        check docker tar name

    .. code-block:: bash

        load ubuntu-ddsvisualizer:<version>.tar

    where ``version`` is the downloaded version of |ddsrouter|.

    |br|

#.  Run the Docker container executing the following command:

    .. code-block:: bash

        docker --net=host ubuntu-ddsvisualizer:<version>

    After executing the previous command you should be able to see the application running.
    Check the following section :ref:`user_manual_start` to learn how to start the |ddsvisualizer|.
