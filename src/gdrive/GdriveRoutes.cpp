#include <QFile>
#include <QFileInfo>
#include "GdriveRoutes.h"
#include "gdrive/files/FilesCreateFileDetails.h"

using namespace googleQt;

GdriveRoutes::GdriveRoutes(Endpoint* e):m_endpoint(e)
{
};

files::FilesRoutes* GdriveRoutes::getFiles()
{
  if(!m_Files){
    m_Files.reset(new files::FilesRoutes(m_endpoint));
  }
  return m_Files.get();
};

about::AboutRoutes* GdriveRoutes::getAbout()
{
  if(!m_About){
    m_About.reset(new about::AboutRoutes(m_endpoint));
  }
  return m_About.get();
};


permissions::PermissionsRoutes* GdriveRoutes::getPermissions() 
{
    if (!m_Permissions) {
        m_Permissions.reset(new permissions::PermissionsRoutes(m_endpoint));
    }
    return m_Permissions.get();
};

comments::CommentsRoutes* GdriveRoutes::getComments()
{
    if (!m_Comments) {
        m_Comments.reset(new comments::CommentsRoutes(m_endpoint));
    }
    return m_Comments.get();
};

revisions::RevisionsRoutes* GdriveRoutes::getRevisions()
{
    if (!m_Revisions) {
        m_Revisions.reset(new revisions::RevisionsRoutes(m_endpoint));
    }
    return m_Revisions.get();
};


QString GdriveRoutes::appDataFolder() 
{
    return "appDataFolder";
};

QString GdriveRoutes::fileExists(QString name, QString parentId)
{
    QString rv = "";

    gdrive::FileListArg arg;
    QString q = QString("name = '%1' and mimeType != 'application/vnd.google-apps.folder' and trashed = false")
        .arg(name);    
    if (!parentId.isEmpty()) 
    {
        q += QString(" and '%1' in parents").arg(parentId);
    }
    arg.setQ(q);

    try
    {
        auto lst = getFiles()->list(arg);
        const std::list <files::FileResource>& files = lst->files();
        if (files.size() > 0) 
        {
            rv = files.cbegin()->id();
        }
    }
    catch (GoogleException& e)
    {
        qWarning() << "GdriveRoutes::fileExists Exception: " << e.what();
    }
    return rv;
};

QString GdriveRoutes::folderExists(QString name, QString parentId)
{
    QString rv = "";

    gdrive::FileListArg arg;
    QString q = QString("name contains '%1' and mimeType = 'application/vnd.google-apps.folder' and trashed = false")
        .arg(name);
    if (!parentId.isEmpty())
    {
        q += QString(" and '%1' in parents").arg(parentId);
    }
    arg.setQ(q);

    try
    {
        auto lst = getFiles()->list(arg);
        const std::list <files::FileResource>& files = lst->files();
        if (files.size() > 0)
        {
            rv = files.cbegin()->id();
        }
    }
    catch (GoogleException& e)
    {
        qWarning() << "GdriveRoutes::folderExists Exception: " << e.what();
    }
    return rv;
};

QString GdriveRoutes::createFolder(QString name, QString parentId /*= ""*/) 
{
    QString rv = "";

    gdrive::CreateFolderArg arg(name);
    arg.setFields("id");
    if (!parentId.isEmpty())
    {
        std::list <QString> parents;
        parents.push_back(parentId);
        arg.setParents(parents);
    }

    try
    {
        auto f = getFiles()->createFolder(arg);
        rv = f->id();
    }
    catch (GoogleException& e)
    {
        qWarning() << "GdriveRoutes::createFolder Exception: " << e.what();
    }
    return rv;
};

QString GdriveRoutes::ensureFolder(QString name, QString parentFolderId)
{
    QString fID = folderExists(name, parentFolderId);
    if (!fID.isEmpty())
    {
        return fID;
    }

    return createFolder(name, parentFolderId);
};

bool GdriveRoutes::downloadFileByID(QString fileId, QString localDestinationPath)
{
    bool rv = false;

    QFile out(localDestinationPath);
    if (!out.open(QFile::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Error opening file: " << localDestinationPath;
        return false;
    }

    try
    {
        gdrive::DownloadFileArg arg(fileId);
        getFiles()->downloadFile(arg, &out);
        out.flush();
        rv = true;
    }
    catch (GoogleException& e)
    {
        qWarning() << "GdriveRoutes::downloadFile Exception: " << e.what();
    }

    out.close();
    return rv;
};

bool GdriveRoutes::downloadFileByName(QString name, QString parentFolderId, QString localDestinationPath)
{
    QString fID = fileExists(name, parentFolderId);
    if (fID.isEmpty())
    {
        return false;
    }
    return downloadFileByID(fID, localDestinationPath);
};

bool GdriveRoutes::deleteFile(QString fileID)
{
    bool rv = false;
    try
    {
        gdrive::DeleteFileArg arg(fileID);
        getFiles()->deleteOperation(arg);
        rv = true;
    }
    catch (GoogleException& e)
    {
        qWarning() << "Delete Exception: " << e.what();
    }
    return rv;
};

bool GdriveRoutes::renameFile(QString fileID, QString newName) 
{
    bool rv = false;
    try
    {
        gdrive::RenameFileArg arg(fileID);
        arg.setName(newName);
        getFiles()->rename(arg);
        rv = true;
    }
    catch (GoogleException& e)
    {
        qWarning() << "Rename Exception: " << e.what();
    }
    return rv;
};

bool GdriveRoutes::moveFile(QString fileID, const std::list<QString>& removeParentFolderIDs,
    const std::list<QString>& addParentFolderIDs) 
{
    bool rv = false;
    try
    {
        gdrive::MoveFileArg arg(fileID);
        arg.setRemoveParents(removeParentFolderIDs);
        arg.setAddParents(addParentFolderIDs);
        getFiles()->moveFile(arg);
        rv = true;
    }
    catch (GoogleException& e)
    {
        qWarning() << "Move Exception: " << e.what();
    }
    return rv;
};

bool GdriveRoutes::moveFile(QString fileID, QString removeParentFolderID, QString addParentFolderID) 
{
    std::list<QString> removeParentFolderIDs;
    std::list<QString> addParentFolderIDs;
    removeParentFolderIDs.push_back(removeParentFolderID);
    addParentFolderIDs.push_back(addParentFolderID);
    return moveFile(fileID, removeParentFolderIDs, addParentFolderIDs);
};

QString GdriveRoutes::uploadFile(QString localFilePath, QString destFileName, QString parentFolderId, QString mimeType)
{
    QString fID = fileExists(destFileName, parentFolderId);
    if (!fID.isEmpty()) 
    {
        if (!deleteFile(fID))
            return "";
    }

    QString rv = uploadFileKeepExisting(localFilePath, destFileName, parentFolderId, mimeType);
    return rv;
};

QString GdriveRoutes::uploadFileKeepExisting(QString localFilePath, QString destFileName, QString parentFolderId, QString mimeType)
{
    QFile file_in(localFilePath);
    if (!file_in.open(QFile::ReadOnly)) {
        qWarning() << "Error opening file: " << localFilePath;
        return "";
    }

    QString rv = "";
    QFileInfo fi(localFilePath);

    try
    {
        gdrive::CreateFileArg arg(destFileName);
        files::CreateFileDetails& file_details = arg.fileDetailes();
        if (!parentFolderId.isEmpty())
        {
            std::list <QString> parent_folders;
            parent_folders.push_back(parentFolderId);
            file_details.setParents(parent_folders);
        }
        if (!mimeType.isEmpty())
        {
            file_details.setMimetype(mimeType);
        }
        else
        {
            arg.calcMimeType();
        }
        auto f = getFiles()->create(arg, &file_in);
        rv = f->id();
    }
    catch (GoogleException& e)
    {
        qWarning() << "Exception: " << e.what();
    }
    file_in.close();
    return rv;
};

QString GdriveRoutes::upgradeFile(QString localFilePath, QString destFileName, QString parentFolderId) 
{
    QString gdTmpFile = destFileName + "._tmp";
    QString fTmpID = uploadFile(localFilePath, gdTmpFile, parentFolderId);
    if (fTmpID.isEmpty()) {
        qWarning() << "Failed to upload file" << localFilePath << destFileName << parentFolderId;
        return "";
    }
    
    QString oldFileID = fileExists(destFileName, parentFolderId);
    if (!oldFileID.isEmpty()) 
    {
        if (!deleteFile(oldFileID)) 
        {   
            qWarning() << "Failed to delete file" << destFileName << parentFolderId << oldFileID;
            return "";
        };
    }

    if (!renameFile(fTmpID, destFileName)) 
    {
        qWarning() << "Failed to rename file" << destFileName << fTmpID;
        return "";
    }

    return fTmpID;
};

QString GdriveRoutes::upgradeAppDataFile(QString localFilePath, QString destFileName)
{
    QString fID = uploadFileKeepExisting(localFilePath, destFileName, "appDataFolder");
    if (fID.isEmpty()) {
        qWarning() << "Failed to upload file" << localFilePath << destFileName << "appDataFolder";
        return "";
    }
    int n = cleanUpAppDataFolder(destFileName, fID);
    qDebug() << "cleaned-up files" << n;
    return fID;
};

int GdriveRoutes::cleanUpAppDataFolder(QString name, QString id2keep)
{
    std::set<QString> names2del;
    std::set<QString> ids2keep;
    names2del.insert(name);
    ids2keep.insert(id2keep);
    return cleanUpAppDataFolder(names2del, ids2keep);
};

int GdriveRoutes::cleanUpAppDataFolder(const std::set<QString>& name2del,
                                       const std::set<QString>& ids2keep)
{
    int rv = 0;
    try
        {
            gdrive::FileListArg arg;
            arg.setSpaces("appDataFolder");
            auto lst = getFiles()->list(arg);
            const std::list <files::FileResource>& files = lst->files();
            for(const files::FileResource& f : files)
                {
                    if(ids2keep.find(f.id()) != ids2keep.end())
                        continue;
                    if(name2del.find(f.name()) != name2del.end())
                        {
                            gdrive::DeleteFileArg darg(f.id());
                            getFiles()->deleteOperation(darg);
                            rv++;
                        }
                }
        }
    catch (GoogleException& e)
        {
            qWarning() << "Exception: " << e.what();
            return -1;
        }    
    return rv;
};

QString  GdriveRoutes::appDataFileExists(QString name)
{
    try
        {
            gdrive::FileListArg arg;
            arg.setSpaces("appDataFolder");
            auto lst = getFiles()->list(arg);
            const std::list <files::FileResource>& files = lst->files();
            for(const files::FileResource& f : files)
                {
                    if(f.name().compare(name) == 0)
                        {
                            return f.id();
                        }
                }
        }
    catch (GoogleException& e)
        {
            qWarning() << "Exception: " << e.what();
            return "";
        }    
    return "";    
};

GdriveRoutes::FolderContentMap GdriveRoutes::mapFolderContent(QString parentId, QString q1 /*= "trashed = false"*/)
{
    FolderContentMap rv;

    gdrive::FileListArg arg;
    QString q = q1;
    if (!parentId.isEmpty())
    {
        if (!q.isEmpty()) 
        {
            q += " and ";
        }
        q += QString("'%1' in parents").arg(parentId);
    }
    arg.setQ(q);
    arg.setPageSize(200);

    try
    {
        auto lst = getFiles()->list(arg);
        const std::list <files::FileResource>& files = lst->files();
        for (auto i = files.begin(); i != files.end(); i++)
        {
            QString id = i->id();
            QString name = i->name();
            rv.name2id[name] = id;
            rv.id2name[id] = name;
        }
    }
    catch (GoogleException& e)
    {
        qWarning() << "GdriveRoutes::mapFolders Exception: " << e.what();
    }
    return rv;
};

GdriveRoutes::FolderContentMap GdriveRoutes::mapFolders(QString parentId)
{
    QString q = QString("mimeType = 'application/vnd.google-apps.folder' and trashed = false");
    return mapFolderContent(parentId, q);    
};

GdriveRoutes::FolderContentMap GdriveRoutes::mapNonFolders(QString parentId)
{
    QString q = QString("mimeType != 'application/vnd.google-apps.folder' and trashed = false");
    return mapFolderContent(parentId, q);
};
