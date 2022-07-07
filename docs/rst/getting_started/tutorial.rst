.. include:: ../exports/alias.include

.. _start_tutorial:

################
Example of usage
################

This example will show how to monitor a DDS network using *eProsima DDS Visualizer Plugin*, by leveraging some of the
features and configuration options this add-on has to offer.

We will make use of `ShapesDemo <https://www.eprosima.com/index.php/products-all/eprosima-shapes-demo>`_, a tool
primarily utilized for testing interoperability between different DDS implementations, as well as to experiment with
different QoS configurations.
Shapes of different types and colors move around the screen, while publishing their coordinates in real time over DDS.

.. contents::
    :local:
    :backlinks: none
    :depth: 2


Prerequisites
^^^^^^^^^^^^^

First of all, you need to follow the steps outlined in the :ref:`Installation Manual <installation_manual_windows>` for
the installation of *eProsima DDS Visualizer Plugin* and all its dependencies.
In addition, please refer to *ShapesDemo* `Installation Manual <https://eprosima-shapes-demo.readthedocs.io/en/latest/installation/windows_binaries.html>`_
and install this application before getting started. Version ``2.7.0`` or above is required in this tutorial.

Both of them, along with other *eProsima* tools and products, are preinstalled in **Fast DDS Suite**, so consider using
:ref:`DDS Visualizer Plugin within Fast DDS Suite <dds_suite>` as it already presents all the elements required to
follow this example.
It may also be useful, though not required, to have a look at *Plotjuggler*'s introductory tutorials available in this
section :ref:`user_manual_tutorials`.


Launch ShapesDemo application
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let us run an instance of *ShapesDemo*, and start publishing under the ``Square`` topic.
Choose the DDS domain on which data will be published (Options->Participant Configuration), and create a publisher with
default settings on this topic.
See `Using eProsima Shapes Demo <https://eprosima-shapes-demo.readthedocs.io/en/latest/first_steps/first_steps.html>`_
for more details.

|domain| |settings| |square|

.. |domain| image:: /rst/figures/usage_example/shapesdemo_domain_selection.png
   :width: 33%

.. |settings| image:: /rst/figures/usage_example/publisher_settings.png
   :width: 33%

.. |square| image:: /rst/figures/usage_example/square_shape.png
   :width: 33%


Visualize DDS data
^^^^^^^^^^^^^^^^^^

Run now *Plotjuggler* application, and :ref:`start streaming <user_manual_start>` data through the |ddsvisualizer|.
In the DDS :ref:`configuration <user_manual_configuration>` settings tab, select the same DDS domain as the one
previously chosen in *ShapesDemo*.

.. figure:: /rst/figures/usage_example/domain_selection.png


A row with the ``Square`` topic should be visible under the *Topic Selector* tab. Select it and hit the ``OK`` button to
proceed.

.. figure:: /rst/figures/usage_example/topic_selection.png


Plotting time series
""""""""""""""""""""

One can now plot, for example, the evolution in time of the ``y`` coordinate. Just drag and drop to the main panel
the corresponding entry appearing on the left column, and data should start being displayed right away.

It is rather confusing to interpret this particular time series, as in *ShapesDemo* the origin for this axis corresponds
to the top of the grid, while in *Plotjuggler* it is depicted the other way around.
This can be easily solved by flipping the Y axis (option available when right clicking over a plot).

.. figure:: /rst/figures/usage_example/y-plot.png


X-Y curves
""""""""""

Still, it is not the most intuitive to interpret both series separately. Let us exploit another useful feature
*Plotjuggler* brings to the table: `x-y curves <https://facontidavide.github.io/PlotJuggler/visualization_howto/index.html#display-a-xy-curve>`_.

Create a new tab, select both ``x`` and ``y`` items from the *Timeseries List*, and drag them to the empty canvas, this
time using the right mouse button.
Again, flip the y axis to visualize the movement of the square shape, now closely resembling the one observed at
*ShapesDemo* application.

.. raw:: html

    <video width=100% height=auto autoplay loop controls>
        <source src="../../_static/usage_example.mp4">
        Your browser does not support the video tag.
    </video>


Next steps
^^^^^^^^^^

We hope this example served as a useful resource for getting a grasp on the basics of *eProsima DDS Visualizer Plugin*.
Feel free to explore the :ref:`User Manual <user_manual_start>` section to learn more about its various features and
configuration options.
