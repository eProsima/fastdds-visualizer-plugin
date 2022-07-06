.. include:: ../exports/alias.include

.. _start_tutorial:

################
Example of usage
################

This example will show how to monitor a DDS network using *eProsima DDS Visualizer Plugin*, by leveraging some of the
features and configuration options this add-on has to offer.

We will make use of `ShapesDemo <https://www.eprosima.com/index.php/products-all/eprosima-shapes-demo>`_, a tool
primarily utilized for testing interoperability between different DDS implementations, as well as to experiment with
different QoS configurations. Shapes of different types and colors move around the screen, while publishing their
coordinates in real time over DDS.

Let us first launch an instance of *ShapesDemo*, and start publishing under the ``Square`` topic.
Choose the DDS domain on which data will be published (Options->Participant Configuration), and create a publisher with
default settings on this topic.

|domain| |settings| |square|

.. |domain| image:: /rst/figures/usage_example/shapesdemo_domain_selection.png
   :width: 33%

.. |settings| image:: /rst/figures/usage_example/publisher_settings.png
   :width: 33%

.. |square| image:: /rst/figures/usage_example/square_shape.png
   :width: 33%

Run now *Plotjuggler* application, and start streaming data through the |ddsvisualizer|. In the DDS configuration
settings tab, select the same DDS domain as the one previously chosen in *ShapesDemo*.

.. figure:: /rst/figures/usage_example/domain_selection.png


Using *Fast-DDS*, there are two different ways in which *Plotjuggler* is able to interpret received data; by having the
publisher send the relevant :ref:`type information <user_manual__dynamic_data_types__automatic_data_type>`, or by
manually including :ref:`xml files <user_manual__dynamic_data_types__xml_data_type>` containing the required
information. In this tutorial, the former mechanism is the one being used. Starting from version ``2.7.0``, *ShapesDemo*
publishes this type information by default, so no further configuration steps need to be taken.

Hence, a row with the ``Square`` topic should be visible under the *Topic Selector* tab. Select it and hit the ``OK``
button to proceed.

.. figure:: /rst/figures/usage_example/topic_selection.png

One can now plot, for example, the evolution in time of the ``y`` coordinate. Just drag and drop to the main panel
the corresponding entry appearing on the left column, and data should start being displayed right away.

It is rather confusing to interpret this particular time series, as in *ShapesDemo* the origin for this axis corresponds
to the top of the grid, while in *Plotjuggler* it is depicted the other way around.
This can be easily solved by flipping the Y axis (option available when right clicking over a plot).

.. figure:: /rst/figures/usage_example/y-plot.png

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

Feel free to explore the :ref:`User Manual <dynamic_data_types_sec>` section to learn more about
*eProsima DDS Visualizer Plugin* features and configuration options.
