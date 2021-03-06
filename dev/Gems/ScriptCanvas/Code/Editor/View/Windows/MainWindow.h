#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QTranslator>
#include <QMimeData>

#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Asset/AssetManagerBus.h>

#include <GraphCanvas/Components/Connections/ConnectionBus.h>
#include <GraphCanvas/Components/SceneBus.h>

#include <Core/Core.h>

#include <ScriptCanvas/Bus/RequestBus.h>
#include <ScriptCanvas/Bus/EditorScriptCanvasBus.h>
#include <ScriptCanvas/Bus/DocumentContextBus.h>

#include <Editor/Assets/ScriptCanvasAssetHolder.h>
#include <Editor/Undo/ScriptCanvasGraphCommand.h>
#include <Editor/Utilities/RecentFiles.h>

#if SCRIPTCANVAS_EDITOR
#include <Include/EditorCoreAPI.h>
#endif

namespace GraphCanvas
{
    class GraphCanvasMimeEvent;
}

namespace Ui
{
    class MainWindow;
}

class QDir;
class QFile;
class QProgressDialog;

namespace ScriptCanvasEditor
{
    enum class UnsavedChangesOptions;
    namespace Widget
    {
        class BusSenderTree;
        class CommandLine;
        class Debugging;
        class GraphTabBar;
        class NodePalette;
        class NodeProperties;
        class NodeOutliner;
        class PropertyGrid;
        class LogPanelWidget;
    }

    class CreateNodeContextMenu;
    class EBusHandlerActionMenu;
    class UndoManager;

    class MainWindow
        : public QMainWindow
        , private UIRequestBus::Handler
        , private GeneralRequestBus::Handler
        , private UndoNotificationBus::Handler
        , private GraphCanvas::SceneNotificationBus::MultiHandler
        , private GraphCanvas::SceneUIRequestBus::MultiHandler
        , private DocumentContextNotificationBus::MultiHandler
#if SCRIPTCANVAS_EDITOR
        //, public IEditorNotifyListener
#endif
    {
        Q_OBJECT
    private:
        friend class EmptyCanvasDropFilter;

    public:

        MainWindow();
        ~MainWindow() override;

    private:
        // UIRequestBus
        QWidget* GetMainWindow() override { return this; }

        // Undo Handlers
        void PostUndoPoint(AZ::EntityId sceneId) override;
        void SignalSceneDirty(const AZ::EntityId& sceneId) override;

        void PushPreventUndoStateUpdate() override;
        void PopPreventUndoStateUpdate() override;
        void ClearPreventUndoStateUpdate() override;
        void CreateUndoManager();
        void TriggerUndo();
        void TriggerRedo();

        // UI Handlers
        void OnConnectionContextMenuEvent(const AZ::EntityId& connectionId, QGraphicsSceneContextMenuEvent* event) override;
        void OnNodeContextMenuEvent(const AZ::EntityId& nodeId, QGraphicsSceneContextMenuEvent* event) override;
        void OnSceneContextMenuEvent(const AZ::EntityId& sceneId, QGraphicsSceneContextMenuEvent* event) override;
        void OnSlotContextMenuEvent(const AZ::EntityId& slotId, QGraphicsSceneContextMenuEvent* event) override;
        void OnSceneDoubleClickEvent(const AZ::EntityId& sceneId, QGraphicsSceneMouseEvent* event) override;
        GraphCanvas::Endpoint CreateNodeForProposal(const AZ::EntityId& connectionId, const GraphCanvas::Endpoint& endpoint, const QPointF& scenePoint, const QPoint& screenPoint) override;
        void OnWrapperNodeActionWidgetClicked(const AZ::EntityId& wrapperNode, const QRect& actionWidgetBoundingRect, const QPointF& scenePoint, const QPoint& screenPoint) override;
        void RequestUndoPoint() override;
        void RequestPushPreventUndoStateUpdate() override;
        void RequestPopPreventUndoStateUpdate() override;

        // File menu
        void OnFileNew();
        bool OnFileSave(const DocumentContextRequests::SaveCB& saveCB);
        bool OnFileSaveAs(const DocumentContextRequests::SaveCB& saveCB);
        bool OnFileSaveCaller(){return OnFileSave(nullptr);};
        bool OnFileSaveAsCaller(){return OnFileSaveAs(nullptr);};
        void OnFileOpen();

        // Edit menu
        void SetupEditMenu();
        void OnEditMenuShow();
        void RefreshPasteAction();
        void RefreshGraphPreferencesAction();
        void OnEditCut();
        void OnEditCopy();
        void OnEditPaste();
        void OnEditDuplicate();
        void OnEditDelete();

        // View menu
        void OnViewNodePalette();
        void OnViewOutline();
        void OnViewProperties();
        void OnViewDebugger();
        void OnViewCommandLine();
        void OnViewLog();
        void OnRestoreDefaultLayout();
        /////////////////////////////////////////////////////////////////////////////////////////////

        //SceneNotificationBus
        void OnSelectionChanged() override;

        void OnNodeAdded(const AZ::EntityId& nodeId) override;
        void OnNodeRemoved(const AZ::EntityId& nodeId) override;
        void OnNodePositionChanged(const AZ::EntityId& nodeId, const AZ::Vector2& position) override;
        /////////////////////////////////////////////////////////////////////////////////////////////

        void SetDefaultLayout();

        void RefreshSelection();
        void Clear();

        void OnTabInserted(int index);
        void OnTabRemoved(int index);
        void OnTabCloseRequest(int index);
        void OnTabCloseButtonPressed(int index);

        bool IsTabOpen(const AZ::Data::AssetId& assetId, int& outTabIndex) const;
        QVariant GetTabData(const AZ::Data::AssetId& assetId);

        //! GeneralRequestBus
        AZ::Outcome<int, AZStd::string> OpenScriptCanvasAsset(const AZ::Data::Asset<ScriptCanvasAsset>& scriptCanvasAsset, int tabIndex = -1, AZ::EntityId hostId = AZ::EntityId()) override;
        int CloseScriptCanvasAsset(const AZ::Data::AssetId& assetId) override;
        ////
        AZ::Outcome<int, AZStd::string> CreateScriptCanvasAsset(AZStd::string_view assetPath, int tabIndex = -1);
        AZ::Outcome<int, AZStd::string> UpdateScriptCanvasAsset(const AZ::Data::Asset<ScriptCanvasAsset>& scriptCanvasAsset);
        
        void RefreshScriptCanvasAsset(const AZ::Data::Asset<ScriptCanvasAsset>& scriptCanvasAsset);

        //! Removes the assetId -> ScriptCanvasAsset mapping and disconnects from the asset
        //! The DocumentContextNotificationBus and the GraphCanvas::SceneNotificationBus
        void RemoveScriptCanvasAsset(const AZ::Data::AssetId& assetId);
        void MoveScriptCanvasAsset(const AZ::Data::Asset<ScriptCanvasAsset>& newAsset, const ScriptCanvasAssetFileInfo& assetFileInfo);

        void LoadStyleSheet(const AZ::EntityId& sceneId);

        void OnChangeActiveGraphTab(const Widget::GraphTabMetadata&) override;
        AZ::EntityId GetActiveGraphId() const override;
        AZ::EntityId GetActiveSceneId() const override;
        AZ::EntityId GetGraphId(const AZ::EntityId& sceneId) const override;
        AZ::EntityId GetSceneId(const AZ::EntityId& graphId) const override;
        ////////////////////////////

        bool eventFilter(QObject *object, QEvent *event) override;

    protected:

        void OnEmptyCanvasDragEnter(QDragEnterEvent* dragEnterEvent);
        void OnEmptyCanvasDrop(QDropEvent* dropEvent);

    private:
        void DeleteNodes(const AZ::EntityId& sceneId, const AZStd::vector<AZ::EntityId>& nodes) override;
        void DeleteConnections(const AZ::EntityId& sceneId, const AZStd::vector<AZ::EntityId>& connections) override;
        void DisconnectEndpoints(const AZ::EntityId& sceneId, const AZStd::vector<GraphCanvas::Endpoint>& endpoints) override;
        /////////////////////////////////////////////////////////////////////////////////////////////

        //! DocumentContextNotificationBus
        void OnScriptCanvasAssetReady(const AZ::Data::Asset<ScriptCanvasAsset>& scriptCanvasAsset) override;
        ////

        void SourceFileChanged(const AZ::Data::AssetId& saveAssetId, AZStd::string relPath, AZStd::string scanFolder, const AZ::Data::AssetId& assetId);

        void CloneAssetEntity(AZ::Data::Asset<ScriptCanvasAsset> scriptCanvasAsset);
        void ActivateAssetEntity(AZ::Data::Asset<ScriptCanvasAsset> scriptCanvasAsset);

        //! UndoNotificationBus
        void OnCanUndoChanged(bool canUndo) override;
        void OnCanRedoChanged(bool canRedo) override;
        ////

        //! Helper function which serializes a file to disk
        //! \param filename name of file to serialize the Entity
        //! \param asset asset to save
        void SaveScriptCanvasAsset(AZStd::string_view filename, AZ::Data::Asset<ScriptCanvasAsset> asset, const DocumentContextRequests::SaveCB& saveCB);
        AZStd::string GetSuggestedFullFilenameToSaveAs(const AZ::Data::AssetId& assetId);

        AZ::Data::Asset<ScriptCanvasAsset> CopyAssetForSave(const AZ::Data::AssetId& newAssetId, AZ::Data::Asset<ScriptCanvasAsset> oldAsset);

        void MarkAssetModified(const AZ::Data::AssetId& assetId);
        void MarkAssetUnmodified(const AZ::Data::AssetId& assetId);

        // QMainWindow
        void closeEvent(QCloseEvent *event) override;
        UnsavedChangesOptions ShowSaveDialog(const QString& filename);
        bool SaveAsset(const AZ::Data::AssetId& unsavedAssetId, const DocumentContextRequests::SaveCB& saveCB);

        void OpenFile(const char* fullPath);
        void CreateMenus();

        void SignalActiveSceneChanged(const AZ::EntityId& sceneId);

        void SaveWindowState();
        void RestoreWindowState();

        void RunBatchConversion();
        void BatchConvertDirectory(QDir directory);
        void BatchConvertFile(const QString& fileName);

        NodeIdPair ProcessCreateNodeMimeEvent(GraphCanvas::GraphCanvasMimeEvent* mimeEvent, const AZ::EntityId& sceneId, AZ::Vector2 nodeCreationPos);

    public slots:
        void UpdateRecentMenu();
    private:
        QWidget* m_host = nullptr;
        Widget::GraphTabBar* m_tabBar = nullptr;
        Widget::NodeOutliner* m_nodeOutliner = nullptr;
        Widget::NodePalette* m_nodePalette = nullptr;
        Widget::Debugging* m_debugging = nullptr;
        Widget::LogPanelWidget* m_logPanel = nullptr;
        Widget::PropertyGrid* m_propertyGrid = nullptr;
        Widget::CommandLine* m_commandLine = nullptr;

        // Reusable context menu for creating nodes.
        CreateNodeContextMenu* m_createNodeContextMenu;
        AZ::EntityId m_entityMimeDelegateId;

        // Reusable context menu for adding/removing ebus events from a wrapper node
        EBusHandlerActionMenu* m_ebusHandlerActionMenu;

        QLabel* m_emptyCanvas; // Displayed when there is no open graph
        QVBoxLayout* m_layout;

        //! \param asset The AssetId of the ScriptCanvas Asset.
        void SetActiveAsset(const AZ::Data::AssetId& assetId);
        void RefreshActiveAsset();

        AZ::Data::AssetId m_activeAssetId;

        struct AssetGraphSceneId
        {
            AZ::Data::AssetId m_assetId;
            AZ::EntityId m_graphId;
            AZ::EntityId m_sceneId;
        };

    public:
        struct AssetGraphSceneData
        {
            AZ_CLASS_ALLOCATOR(AssetGraphSceneData, AZ::SystemAllocator, 0);
            AssetGraphSceneData(const AZ::Data::Asset<ScriptCanvasAsset>& scriptCanvasAsset);
            AssetGraphSceneData(const AssetGraphSceneData&) = delete;
            AssetGraphSceneData& operator=(const AssetGraphSceneData&) = delete;
            void Set(const AZ::Data::Asset<ScriptCanvasAsset>& scriptCanvasAsset);

            AssetGraphSceneId m_tupleId;
            AZ::Data::Asset<ScriptCanvasAsset> m_asset; //< Maintains a reference to ScriptCanvasAsset that is open within the editor
                                                   //< While a reference is maintained in the MainWindow, the ScriptCanvas Graph will remain open
                                                   //< Even if the underlying asset is removed from disk
        };

    private:

        /*! AssetGraphSceneMapper
        The AssetGraphSceneMapper is a structure for allowing quick lookups of open ScriptCanvas
        graphs assetIds, GraphCanvas Scene EntityIds, ScriptCanvas Graphs Unique EntityIds and the ScriptCanvasAsset object
        */
        struct AssetGraphSceneMapper
        {
            AZ::Outcome<void, AZStd::string> Add(AZ::Data::Asset<ScriptCanvasAsset> scriptCanvasAsset);
            void Remove(const AZ::Data::AssetId& assetId);
            void Clear();

            AssetGraphSceneData* GetByAssetId(const AZ::Data::AssetId& assetId) const;
            AssetGraphSceneData* GetByGraphId(AZ::EntityId graphId) const;
            AssetGraphSceneData* GetBySceneId(AZ::EntityId entityId) const;

            AZStd::unordered_map<AZ::Data::AssetId, AZStd::unique_ptr<AssetGraphSceneData>> m_assetIdToDataMap;
        };
        AssetGraphSceneMapper m_assetGraphSceneMapper;

        bool m_enterState;
        bool m_ignoreSelection;
        AZ::s32 m_preventUndoStateUpdateCount;

        Ui::MainWindow* ui;
        AZStd::array<AZStd::pair<QAction*, QMetaObject::Connection>, c_scriptCanvasEditorSettingsRecentFilesCountMax> m_recentActions;
        QHBoxLayout *m_horizontalTabBarLayout;
        QSpacerItem* m_plusButtonSpacer;
        AZStd::unique_ptr<UndoManager> m_undoManager;

        QByteArray m_defaultLayout;
        QTranslator m_translator;
        
        QMimeData   m_initialDropMimeData;

        // Batch Conversion Information
        bool m_processing;
        QProgressDialog* m_progressDialog;
        size_t m_processCount;
        AZStd::unordered_set< AZ::Data::AssetId > m_savingIds;
    };
}
