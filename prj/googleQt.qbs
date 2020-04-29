import qbs
import qbs.FileInfo


Project {
    Product {
        name: "googleQt"
        property bool dynamicBuild: true
        type: dynamicBuild?"dynamiclibrary":"staticlibrary";
        //type: "staticlibrary";

        Depends { name: "cpp" }
        Depends { name: "Qt.network" }
        Depends { name: "Qt.xml" }
        Depends { name: "Qt.sql" }
        Depends { name: "Qt.gui" }

        cpp.cxxLanguageVersion: "c++14"
        cpp.includePaths: [
            "../src",
        ]
        cpp.defines: [
          "API_QT_DIAGNOSTICS",
        ].concat((dynamicBuild)?["GOOGLEQT_DLL"]:[])

        property stringList paths: [
          "../src",
          "../src/gmail",
          "../src/gmail/users",
          "../src/gmail/messages",
          "../src/gmail/labels",
          "../src/gmail/threads",
          "../src/gmail/drafts",
          "../src/gmail/errors",
          "../src/gmail/history",
          "../src/gmail/attachments",
          "../src/gtask",
          "../src/gtask/tasks",
          "../src/gtask/tasklists",
          "../src/gdrive",
          "../src/gdrive/about",
          "../src/gdrive/files",
          "../src/gdrive/permissions",
          "../src/gdrive/comments",
          "../src/gdrive/revisions",
          "../src/gcontact",
          "../src/gcontact/contacts",
          "../src/gcontact/contact_group",
          "../src/google/endpoint",
          "../src/google/demo",
        ]

        files: {
          var list = [];
          for(var i in paths) {
            list.push(FileInfo.joinPaths(paths[i], "*.cpp"));
            list.push(FileInfo.joinPaths(paths[i], "*.h"));
          }

          return list;
        }

        Group {
            fileTagsFilter: ["dynamiclibrary", "dynamiclibrary_symlink"]
                        .concat(qbs.buildVariant === "debug" ? ["debuginfo_dll"] : [])
            qbs.install: (dynamicBuild);
            qbs.installDir: "bin"
        }

        Group {
            fileTagsFilter: ["dynamiclibrary_import"]
            qbs.install: (dynamicBuild);
            qbs.installDir: "lib"
        }

        FileTagger {
          patterns: "*.h"
          fileTags: "headers"
        }

        Group {
          name: "Headers"
          fileTagsFilter: ["headers"]
          qbs.install: (dynamicBuild);
          qbs.installDir: "include"
          qbs.installSourceBase: "../src/" //Preserve subdirectories.
        }
        qbs.installPrefix: ""
    }
}
