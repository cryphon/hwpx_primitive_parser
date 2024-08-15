#include <iostream>
#include <QApplication>
#include <QTextEdit>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <libxml/parser.h>
#include <zip.h>


// parse XML content from extracted file
QString parseXML(const char *filename) {
    QString parsedText;

    xmlDoc *doc = xmlReadFile(filename, NULL, 0);

    if(doc == NULL) {
        parsedText = "Failed to parse XML";
        return parsedText;
    }

    xmlNode *root = xmlDocGetRootElement(doc);
    for(xmlNode *node = root->children; node; node = node->next) {
        if(node->type == XML_ELEMENT_NODE && xmlNodeGetContent(node)) {
            parsedText += QString::fromUtf8((const char*)xmlNodeGetContent(node)) + "\n";
        }
    }

    xmlFreeDoc(doc);
    return parsedText;

}


QString extractAndParseHWPX(const QString &filepath) {
    QString extractedContent;
    int err = 0;
    zip_t *zip = zip_open(filepath.toStdString().c_str(), ZIP_RDONLY, &err);

    if(zip == nullptr) {
        return "Failed to open HWPX";
    }

    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(zip, "Contents/section0.xml", 0, &st);

    char *contents = new char[st.size];
    zip_file_t *zf = zip_fopen(zip, "Contents/section0.xml", 0);
    if(zf == nullptr) {
        int zip_err;
        zip_error_t *error = zip_get_error(zip);
        zip_err = zip_error_code_zip(error);
        std::cerr << "Error opening file: " << zip_strerror(zip) << " (error code: " << zip_err << ")" << std::endl;
        printf("contents: %s", contents);
        return "Failed to open content.xml in HWPX file.";
    }

    zip_fread(zf, contents, st.size);
    zip_fclose(zf);

   QString tempFileName = "temp_content.xml";
    QFile tempFile(tempFileName);
    if (tempFile.open(QIODevice::WriteOnly)) {
        tempFile.write(contents, st.size);
        tempFile.close();
    }

    extractedContent = parseXML(tempFileName.toStdString().c_str());
    
    // Clean up
    delete[] contents;
    zip_close(zip);
    return extractedContent;
}





int main(int argc, char *argv[]) {
    std::cout << "Hello world!" << std::endl;

    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("HWPX viewer");

    QVBoxLayout *layout = new QVBoxLayout;
    QTextEdit *textEdit = new QTextEdit;

    QPushButton *openButton = new QPushButton("Open HWPX File");


    layout->addWidget(openButton);
    layout->addWidget(textEdit);

    window.setLayout(layout);

    QObject::connect(openButton, &QPushButton::clicked, [&]() {
            QString filename = QFileDialog::getOpenFileName(&window, "Open HWPX File", "", "HWPX Files (*hwpx)");
            if(!filename.isEmpty()) {
            QString content = extractAndParseHWPX(filename);
            textEdit->setText(content);
        }
    });

    window.show();
    return app.exec();
}

