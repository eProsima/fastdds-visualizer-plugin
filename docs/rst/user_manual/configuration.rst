.. include:: ../exports/alias.include

.. _user_manual_configuration:

###################################
DDS Visualizer Plugin Configuration
###################################

Once the plugin is started, a new dialog will open where choosing the configuration for the internal |fastdds|
*Domain Participant*.
This section explains the different configuration options.

.. todo::
    Add a link to the start plugin


Topic Selector
==============

.. todo::
    Add screenshot of Topic Selector marking tab

In order for |ddsvisualizer| to receive data from a DDS topic, it is required to select the DDS topics.
In each of the topics selected, the |ddsvisualizer| will create a *DataReader* and receive the data,
allowing to plot it afterwards.
The following capture shows the *topic selector* dialog.

Select topics
-------------

.. todo::
    Add screenshot of Topic Selector with at least 3 topics. 1 selected, 1 unselected and 1 disabled

Select all topics
^^^^^^^^^^^^^^^^^

In order to select all possible topics discovered, presh :code:`Ctrl+A`.
In order to deselect all topics, presh :code:`Ctrl+Shift+A`.

Filter topics
-------------

There is a filter in the upper right corner of the tab that allows to filter topics by name.
Writing plain text in there will hide those topics that do not contain such text in their names.
Be aware that those topics that are hidden will still be selected, if they were before filtering.

.. todo::
    Add screenshot of Topic Selector marking filter bar


DDS Configuration
=================

.. todo::
    Add screenshot of DDS Configuration marking tab

In this tab is configured the internal *Domain Participant* of the |ddsvisualizer|.

DDS Domain Id
-------------

To select the
`DDS Domain Id <https://fast-dds.docs.eprosima.com/en/latest/fastdds/dds_layer/domain/domain.html?highlight=Domain>`_
of the internal |fastdds| *Domain Participant*, press :code:`Change Domain` button.
This pops up a new Dialog to choose a new *Domain Id*.

.. todo::
    Add screenshot of Change DomainId dialog

XML Data Types
--------------

In order to explicitly specify a Data Type for a topic, XML files with data type descriptions could be uploaded.
Using the buttons :code:`Include File` or :code:`Include Directory` the user can select an XML file or all
the XML files in a directory, and they will be parsed by |fastdds| and their data types will be known for
the application hereinafter.
Check section :ref:`user_manual__dynamic_data_types__xml_data_type` for more information.


Advance Options
===============
