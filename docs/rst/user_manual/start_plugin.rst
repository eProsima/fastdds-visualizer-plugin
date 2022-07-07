.. include:: ../exports/alias.include

.. _user_manual_start:

################################
Start Fast DDS Visualizer Plugin
################################

In order to start the |ddsvisualizer| in the |plotjuggler| application,
select in combo box in *Streaming* section in left panel the plugin *Fast DDS DataStreamer Plugin*.

.. figure:: /rst/figures/start_plugin_edited.png

.. note::

    If this plugin does not automatically appear in the combo box, it means the plugin has not been correctly loaded.
    Check section :ref:`source_installation_source_plugin` to learn how to add the plugin to the application.
    Be sure that |fastdds| libraries are sourced.

Once this plugin is selected, press *Start* button and a new Dialog will appear asking for plugin configurations.
To learn more about these configurations, check section :ref:`user_manual_configuration`.

Stop plugin
===========

User can press *Stop* button in *Streaming* section at any time to stop the plugin
and destroy every DDS entity internally created.
