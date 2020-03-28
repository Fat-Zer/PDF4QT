//    Copyright (C) 2020 Jakub Melka
//
//    This file is part of PdfForQt.
//
//    PdfForQt is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    PdfForQt is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with PDFForQt. If not, see <https://www.gnu.org/licenses/>.

#ifndef PDFDOCUMENTBUILDER_H
#define PDFDOCUMENTBUILDER_H

#include "pdfobject.h"
#include "pdfdocument.h"
#include "pdfannotation.h"

namespace pdf
{

struct WrapName
{
    WrapName(const char* name) :
        name(name)
    {

    }

    QByteArray name;
};

struct WrapAnnotationColor
{
    WrapAnnotationColor(QColor color) :
        color(color)
    {

    }

    QColor color;
};

struct WrapFreeTextAlignment
{
    constexpr inline WrapFreeTextAlignment(Qt::Alignment alignment) :
        alignment(alignment)
    {

    }

    Qt::Alignment alignment;
};

struct WrapString
{
    WrapString(const char* string) :
        string(string)
    {

    }

    QByteArray string;
};

struct WrapCurrentDateTime { };
struct WrapEmptyArray { };

/// Factory for creating various PDF objects, such as simple objects,
/// dictionaries, arrays etc.
class PDFObjectFactory
{
public:
    inline explicit PDFObjectFactory() = default;

    void beginArray();
    void endArray();

    void beginDictionary();
    void endDictionary();

    void beginDictionaryItem(const QByteArray& name);
    void endDictionaryItem();

    PDFObjectFactory& operator<<(std::nullptr_t);
    PDFObjectFactory& operator<<(bool value);
    PDFObjectFactory& operator<<(PDFReal value);
    PDFObjectFactory& operator<<(PDFInteger value);
    PDFObjectFactory& operator<<(PDFObjectReference value);
    PDFObjectFactory& operator<<(WrapName wrapName);
    PDFObjectFactory& operator<<(int value);
    PDFObjectFactory& operator<<(const QRectF& value);
    PDFObjectFactory& operator<<(WrapCurrentDateTime);
    PDFObjectFactory& operator<<(WrapAnnotationColor color);
    PDFObjectFactory& operator<<(QString textString);
    PDFObjectFactory& operator<<(WrapEmptyArray);
    PDFObjectFactory& operator<<(TextAnnotationIcon icon);
    PDFObjectFactory& operator<<(LinkHighlightMode mode);
    PDFObjectFactory& operator<<(WrapFreeTextAlignment alignment);
    PDFObjectFactory& operator<<(WrapString string);
    PDFObjectFactory& operator<<(AnnotationLineEnding lineEnding);
    PDFObjectFactory& operator<<(const QPointF& point);
    PDFObjectFactory& operator<<(const QDateTime& dateTime);

    /// Treat containers - write them as array
    template<typename Container, typename ValueType = decltype(*std::begin(std::declval<Container>()))>
    PDFObjectFactory& operator<<(Container container)
    {
        beginArray();

        auto it = std::begin(container);
        auto itEnd = std::end(container);
        for (; it != itEnd; ++it)
        {
            *this << *it;
        }

        endArray();

        return *this;
    }

    PDFObject takeObject();

private:
    void addObject(PDFObject object);

    enum class ItemType
    {
        Object,
        Dictionary,
        DictionaryItem,
        Array
    };

    /// What is stored in this structure, depends on the type.
    /// If type is 'Object', then single simple object is in object,
    /// if type is dictionary, then PDFDictionary is stored in object,
    /// if type is dictionary item, then object and item name is stored
    /// in the data, if item is array, then array is stored in the data.
    struct Item
    {
        inline Item() = default;

        template<typename T>
        inline Item(ItemType type, T&& data) :
            type(type),
            object(qMove(data))
        {

        }

        template<typename T>
        inline Item(ItemType type, const QByteArray& itemName, T&& data) :
            type(type),
            itemName(qMove(itemName)),
            object(qMove(data))
        {

        }

        ItemType type = ItemType::Object;
        QByteArray itemName;
        std::variant<PDFObject, PDFArray, PDFDictionary> object;
    };

    std::vector<Item> m_items;
};

class PDFFORQTLIBSHARED_EXPORT PDFDocumentBuilder
{
public:
    /// Creates a new blank document (with no pages)
    explicit PDFDocumentBuilder();

    ///
    explicit PDFDocumentBuilder(const PDFDocument* document);

    /// Resets the object to the initial state.
    /// \warning All data are lost
    void reset();

    /// Create a new blank document with no pages. If some document
    /// is edited at call of this function, then it is lost.
    void createDocument();

    /// Builds a new document. This function can throw exceptions,
    /// if document being built was invalid.
    PDFDocument build();

    /// If object is reference, the dereference attempt is performed
    /// and object is returned. If it is not a reference, then self
    /// is returned. If dereference attempt fails, then null object
    /// is returned (no exception is thrown).
    const PDFObject& getObject(const PDFObject& object) const;

    /// Returns dictionary from an object. If object is not a dictionary,
    /// then nullptr is returned (no exception is thrown).
    const PDFDictionary* getDictionaryFromObject(const PDFObject& object) const;

    /// Returns object by reference. If dereference attempt fails, then null object
    /// is returned (no exception is thrown).
    const PDFObject& getObjectByReference(PDFObjectReference reference) const;

    /// Returns annotation bounding rectangle
    std::array<PDFReal, 4> getAnnotationReductionRectangle(const QRectF& boundingRect, const QRectF& innerRect) const;

/* START GENERATED CODE */

    /// Appends a new page after last page.
    /// \param mediaBox Media box of the page (size of paper)
    PDFObjectReference appendPage(QRectF mediaBox);


    /// Creates URI action.
    /// \param URL Target URL
    PDFObjectReference createActionURI(QString URL);


    /// Circle annotation displays ellipse (or circle). Circle border/fill color can be defined, along with 
    /// border width. Popup annotation can be attached to this annotation.
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is circle/ellipse displayed
    /// \param borderWidth Width of the border line of circle/ellipse
    /// \param fillColor Fill color of rectangle (interior color). If you do not want to have area color filled, 
    ///        then use invalid QColor.
    /// \param strokeColor Stroke color (color of the rectangle border). If you do not want to have a 
    ///        border, then use invalid QColor.
    /// \param title Title (it is displayed as title of popup window)
    /// \param subject Subject (short description of the subject being adressed by the annotation)
    /// \param contents Contents (text displayed, for example, in the marked annotation dialog)
    PDFObjectReference createAnnotationCircle(PDFObjectReference page,
                                              QRectF rectangle,
                                              PDFReal borderWidth,
                                              QColor fillColor,
                                              QColor strokeColor,
                                              QString title,
                                              QString subject,
                                              QString contents);


    /// Free text annotation displays text directly on a page. Text appears directly on the page, in the 
    /// same way, as standard text in PDF document. Free text annotations are usually used to comment 
    /// the document. Free text annotation can also have callout line, with, or without a knee.
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is text displayed
    /// \param title Title
    /// \param subject Subject
    /// \param contents Contents (text displayed)
    /// \param textAlignment Text alignment. Only horizontal alignment flags are valid.
    PDFObjectReference createAnnotationFreeText(PDFObjectReference page,
                                                QRectF rectangle,
                                                QString title,
                                                QString subject,
                                                QString contents,
                                                TextAlignment textAlignment);


    /// Free text annotation displays text directly on a page. Text appears directly on the page, in the 
    /// same way, as standard text in PDF document. Free text annotations are usually used to comment 
    /// the document. Free text annotation can also have callout line, with, or without a knee. Specify 
    /// start/end point parameters of this function to get callout line.
    /// \param page Page to which is annotation added
    /// \param boundingRectangle Bounding rectangle of free text annotation. It must contain both 
    ///        callout line and text rectangle.
    /// \param textRectangle Rectangle with text, in absolute coordinates. They are then recomputed to 
    ///        match bounding rectangle.
    /// \param title Title
    /// \param subject Subject
    /// \param contents Contents (text displayed)
    /// \param textAlignment Text alignment. Only horizontal alignment flags are valid.
    /// \param startPoint Start point of callout line
    /// \param endPoint End point of callout line
    /// \param startLineType Line ending at the start point
    /// \param endLineType Line ending at the end point
    PDFObjectReference createAnnotationFreeText(PDFObjectReference page,
                                                QRectF boundingRectangle,
                                                QRectF textRectangle,
                                                QString title,
                                                QString subject,
                                                QString contents,
                                                TextAlignment textAlignment,
                                                QPointF startPoint,
                                                QPointF endPoint,
                                                AnnotationLineEnding startLineType,
                                                AnnotationLineEnding endLineType);


    /// Text markup annotation is used to highlight text. It is a markup annotation, so it can contain 
    /// window to be opened (and commented). This annotation is usually used to highlight text, but can 
    /// also highlight other things, such as images, or other graphics.
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is highlight displayed
    /// \param color Color
    /// \param title Title
    /// \param subject Subject
    /// \param contents Contents
    PDFObjectReference createAnnotationHighlight(PDFObjectReference page,
                                                 QRectF rectangle,
                                                 QColor color,
                                                 QString title,
                                                 QString subject,
                                                 QString contents);


    /// Text markup annotation is used to highlight text. It is a markup annotation, so it can contain 
    /// window to be opened (and commented). This annotation is usually used to highlight text, but can 
    /// also highlight other things, such as images, or other graphics.
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is highlight displayed
    /// \param color Color
    PDFObjectReference createAnnotationHighlight(PDFObjectReference page,
                                                 QRectF rectangle,
                                                 QColor color);


    /// Line annotation represents straight line, or some more advanced graphics, such as dimension with 
    /// text. Line annotations are markup annotations, so they can have popup window. Line endings can 
    /// be specified.
    /// \param page Page to which is annotation added
    /// \param boundingRect Line annotation bounding rectangle
    /// \param startPoint Line start
    /// \param endPoint Line end
    /// \param lineWidth Line width
    /// \param fillColor Fill color of line parts (for example, filled line endings)
    /// \param strokeColor Line stroke color
    /// \param title Title (it is displayed as title of popup window)
    /// \param subject Subject (short description of the subject being adressed by the annotation)
    /// \param contents Contents (text displayed, for example, in the marked annotation dialog)
    /// \param startLineType Start line ending type
    /// \param endLineType End line ending type
    PDFObjectReference createAnnotationLine(PDFObjectReference page,
                                            QRectF boundingRect,
                                            QPointF startPoint,
                                            QPointF endPoint,
                                            PDFReal lineWidth,
                                            QColor fillColor,
                                            QColor strokeColor,
                                            QString title,
                                            QString subject,
                                            QString contents,
                                            AnnotationLineEnding startLineType,
                                            AnnotationLineEnding endLineType);


    /// Line annotation represents straight line, or some more advanced graphics, such as dimension with 
    /// text. Line annotations are markup annotations, so they can have popup window. Line endings can 
    /// be specified.
    /// \param page Page to which is annotation added
    /// \param boundingRect Line annotation bounding rectangle
    /// \param startPoint Line start
    /// \param endPoint Line end
    /// \param lineWidth Line width
    /// \param fillColor Fill color of line parts (for example, filled line endings)
    /// \param strokeColor Line stroke color
    /// \param title Title (it is displayed as title of popup window)
    /// \param subject Subject (short description of the subject being adressed by the annotation)
    /// \param contents Contents (text displayed, for example, in the marked annotation dialog)
    /// \param startLineType Start line ending type
    /// \param endLineType End line ending type
    /// \param leaderLineLength Length of the leader line. Leader line extends from each endpoint of 
    ///        the line perpendicular to the line itself. Value can be either positive, negative or zero. If 
    ///        positive, then extension is in plane that is above the annotation line (in clockwise order), 
    ///        if negative, then it is below the annotation line.
    /// \param leaderLineOffset Length of leader line offset, which is the amount of empty space 
    ///        between the endpoints of the annotation and beginning of leader lines
    /// \param leaderLineExtension Length of leader line extension, which extends leader lines in 180° 
    ///        direction from leader lines (so leader lines continues above drawn line)
    /// \param displayContents Display contents of the annotation as text along the line
    /// \param displayedContentsTopAlign Displayed contents appear above the line, instead inline.
    PDFObjectReference createAnnotationLine(PDFObjectReference page,
                                            QRectF boundingRect,
                                            QPointF startPoint,
                                            QPointF endPoint,
                                            PDFReal lineWidth,
                                            QColor fillColor,
                                            QColor strokeColor,
                                            QString title,
                                            QString subject,
                                            QString contents,
                                            AnnotationLineEnding startLineType,
                                            AnnotationLineEnding endLineType,
                                            PDFReal leaderLineLength,
                                            PDFReal leaderLineOffset,
                                            PDFReal leaderLineExtension,
                                            bool displayContents,
                                            bool displayedContentsTopAlign);


    /// Creates new link annotation. It usually represents clickable hypertext link. User can also specify 
    /// action, which can be executed, for example, link can be also in the PDF document (link to some 
    /// location in document).
    /// \param page Page to which is annotation added
    /// \param linkRectangle Link rectangle
    /// \param action Action to be performed when user clicks on a link
    /// \param highlightMode Highlight mode
    PDFObjectReference createAnnotationLink(PDFObjectReference page,
                                            QRectF linkRectangle,
                                            PDFObjectReference action,
                                            LinkHighlightMode highlightMode);


    /// Creates new link annotation. It usually represents clickable hypertext link. User can also specify 
    /// action, which can be executed, for example, link can be also in the PDF document (link to some 
    /// location in document).
    /// \param page Page to which is annotation added
    /// \param linkRectangle Link rectangle
    /// \param URL URL to be launched when user clicks on the link
    /// \param highlightMode Highlight mode
    PDFObjectReference createAnnotationLink(PDFObjectReference page,
                                            QRectF linkRectangle,
                                            QString URL,
                                            LinkHighlightMode highlightMode);


    /// Polygon annotation. When opened, they display pop-up window containing the text of associated 
    /// note (and window title), if popup annotation is attached. Polygon border/fill color can be defined, 
    /// along with border width.
    /// \param page Page to which is annotation added
    /// \param polygon Polygon
    /// \param borderWidth Border line width
    /// \param fillColor Fill color
    /// \param strokeColor Stroke color
    /// \param title Title
    /// \param subject Subject
    /// \param contents Contents
    PDFObjectReference createAnnotationPolygon(PDFObjectReference page,
                                               QPolygonF polygon,
                                               PDFReal borderWidth,
                                               QColor fillColor,
                                               QColor strokeColor,
                                               QString title,
                                               QString subject,
                                               QString contents);


    /// Polyline annotation. When opened, they display pop-up window containing the text of associated 
    /// note (and window title), if popup annotation is attached. Polyline border/fill color can be defined, 
    /// along with border width.
    /// \param page Page to which is annotation added
    /// \param polyline Polyline
    /// \param borderWidth Border line width
    /// \param fillColor Fill color
    /// \param strokeColor Stroke color
    /// \param title Title
    /// \param subject Subject
    /// \param contents Contents
    /// \param startLineType Start line ending type
    /// \param endLineType End line ending type
    PDFObjectReference createAnnotationPolyline(PDFObjectReference page,
                                                QPolygonF polyline,
                                                PDFReal borderWidth,
                                                QColor fillColor,
                                                QColor strokeColor,
                                                QString title,
                                                QString subject,
                                                QString contents,
                                                AnnotationLineEnding startLineType,
                                                AnnotationLineEnding endLineType);


    /// Creates a new popup annotation on the page. Popup annotation is represented usually by floating 
    /// window, which can be opened, or closed. Popup annotation is associated with parent annotation, 
    /// which can be usually markup annotation. Popup annotation displays parent annotation's texts, for 
    /// example, title, comment, date etc.
    /// \param page Page to which is annotation added
    /// \param parentAnnotation Parent annotation (for which is popup window displayed)
    /// \param rectangle Area on the page, where popup window appears
    /// \param opened Is the window opened?
    PDFObjectReference createAnnotationPopup(PDFObjectReference page,
                                             PDFObjectReference parentAnnotation,
                                             QRectF rectangle,
                                             bool opened);


    /// Square annotation displays rectangle (or square). When opened, they display pop-up window 
    /// containing the text of associated note (and window title), if popup annotation is attached. Square 
    /// border/fill color can be defined, along with border width.
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is rectangle displayed
    /// \param borderWidth Width of the border line of rectangle
    /// \param fillColor Fill color of rectangle (interior color). If you do not want to have area color filled, 
    ///        then use invalid QColor.
    /// \param strokeColor Stroke color (color of the rectangle border). If you do not want to have a 
    ///        border, then use invalid QColor.
    /// \param title Title (it is displayed as title of popup window)
    /// \param subject Subject (short description of the subject being adressed by the annotation)
    /// \param contents Contents (text displayed, for example, in the marked annotation dialog)
    PDFObjectReference createAnnotationSquare(PDFObjectReference page,
                                              QRectF rectangle,
                                              PDFReal borderWidth,
                                              QColor fillColor,
                                              QColor strokeColor,
                                              QString title,
                                              QString subject,
                                              QString contents);


    /// Text markup annotation is used to squiggly underline text. It is a markup annotation, so it can 
    /// contain window to be opened (and commented).
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is markup displayed
    /// \param color Color
    /// \param title Title
    /// \param subject Subject
    /// \param contents Contents
    PDFObjectReference createAnnotationSquiggly(PDFObjectReference page,
                                                QRectF rectangle,
                                                QColor color,
                                                QString title,
                                                QString subject,
                                                QString contents);


    /// Text markup annotation is used to squiggly underline text. It is a markup annotation, so it can 
    /// contain window to be opened (and commented).
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is markup displayed
    /// \param color Color
    PDFObjectReference createAnnotationSquiggly(PDFObjectReference page,
                                                QRectF rectangle,
                                                QColor color);


    /// Text markup annotation is used to strikeout text. It is a markup annotation, so it can contain 
    /// window to be opened (and commented).
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is markup displayed
    /// \param color Color
    /// \param title Title
    /// \param subject Subject
    /// \param contents Contents
    PDFObjectReference createAnnotationStrikeout(PDFObjectReference page,
                                                 QRectF rectangle,
                                                 QColor color,
                                                 QString title,
                                                 QString subject,
                                                 QString contents);


    /// Text markup annotation is used to strikeout text. It is a markup annotation, so it can contain 
    /// window to be opened (and commented).
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is markup displayed
    /// \param color Color
    PDFObjectReference createAnnotationStrikeout(PDFObjectReference page,
                                                 QRectF rectangle,
                                                 QColor color);


    /// Creates text annotation. Text annotation is "sticky note" attached to a point in the PDF document. 
    /// When closed, it is displayed as icon, if opened, widget appears with attached text. Text annotations 
    /// do not scale or rotate, they appear independent of zoom/rotate. So, they behave as if flags 
    /// NoZoom or NoRotate to the annotations are being set. Popup annotation is automatically created 
    /// for this annotation.
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is icon displayed
    /// \param iconType Icon type
    /// \param title Title (it is displayed as title of popup window)
    /// \param subject Subject (short description of the subject being adressed by the annotation)
    /// \param contents Contents (text displayed, for example, in the marked annotation dialog)
    /// \param open Is annotation initially displayed as opened?
    PDFObjectReference createAnnotationText(PDFObjectReference page,
                                            QRectF rectangle,
                                            TextAnnotationIcon iconType,
                                            QString title,
                                            QString subject,
                                            QString contents,
                                            bool open);


    /// Text markup annotation is used to underline text. It is a markup annotation, so it can contain 
    /// window to be opened (and commented).
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is markup displayed
    /// \param color Color
    /// \param title Title
    /// \param subject Subject
    /// \param contents Contents
    PDFObjectReference createAnnotationUnderline(PDFObjectReference page,
                                                 QRectF rectangle,
                                                 QColor color,
                                                 QString title,
                                                 QString subject,
                                                 QString contents);


    /// Text markup annotation is used to underline text. It is a markup annotation, so it can contain 
    /// window to be opened (and commented).
    /// \param page Page to which is annotation added
    /// \param rectangle Area in which is markup displayed
    /// \param color Color
    PDFObjectReference createAnnotationUnderline(PDFObjectReference page,
                                                 QRectF rectangle,
                                                 QColor color);


    /// Creates empty catalog. This function is used, when a new document is being created. Do not call 
    /// this function manually.
    PDFObjectReference createCatalog();


    /// Creates page tree root for the catalog. This function is only called when new document is being 
    /// created. Do not call this function manually.
    PDFObjectReference createCatalogPageTreeRoot();


    /// This function is used to create a new trailer dictionary, when blank document is created. Do not 
    /// call this function manually.
    /// \param catalog Reference to document catalog
    PDFObject createTrailerDictionary(PDFObjectReference catalog);


    /// Set document author.
    /// \param author Author
    void setDocumentAuthor(QString author);


    /// Set document creation date.
    /// \param creationDate Creation date/time
    void setDocumentCreationDate(QDateTime creationDate);


    /// Set document creator.
    /// \param creator Creator
    void setDocumentCreator(QString creator);


    /// Set document keywords.
    /// \param keywords Keywords
    void setDocumentKeywords(QString keywords);


    /// Set document producer.
    /// \param producer Producer
    void setDocumentProducer(QString producer);


    /// Set document subject.
    /// \param subject Subject
    void setDocumentSubject(QString subject);


    /// Set document title.
    /// \param title Title
    void setDocumentTitle(QString title);


    /// Set document language.
    /// \param language Document language. It should be a language identifier, as defined in ISO 639 
    ///        and ISO 3166. For example, "en-US", where first two letter means language code (en = 
    ///        english), and the latter two is country code (US - United States).
    void setLanguage(QString language);


    /// Set document language.
    /// \param locale Locale, from which is language determined
    void setLanguage(QLocale locale);


    /// This function is used to update trailer dictionary. Must be called each time the final document is 
    /// being built.
    /// \param objectCount Number of objects (including empty ones)
    void updateTrailerDictionary(PDFInteger objectCount);


/* END GENERATED CODE */

private:
    PDFObjectReference addObject(PDFObject object);
    void mergeTo(PDFObjectReference reference, PDFObject object);
    void appendTo(PDFObjectReference reference, PDFObject object);
    QRectF getPopupWindowRect(const QRectF& rectangle) const;
    QString getProducerString() const;
    PDFObjectReference getPageTreeRoot() const;
    PDFInteger getPageTreeRootChildCount() const;
    PDFObjectReference getDocumentInfo() const;
    PDFObjectReference getCatalogReference() const;
    void updateDocumentInfo(PDFObject info);

    PDFObjectStorage m_storage;
    PDFVersion m_version;
};

// Implementation

inline
const PDFObject& PDFDocumentBuilder::getObject(const PDFObject& object) const
{
    if (object.isReference())
    {
        // Try to dereference the object
        return m_storage.getObject(object.getReference());
    }

    return object;
}

inline
const PDFDictionary* PDFDocumentBuilder::getDictionaryFromObject(const PDFObject& object) const
{
    const PDFObject& dereferencedObject = getObject(object);
    if (dereferencedObject.isDictionary())
    {
        return dereferencedObject.getDictionary();
    }
    else if (dereferencedObject.isStream())
    {
        return dereferencedObject.getStream()->getDictionary();
    }

    return nullptr;
}

inline
const PDFObject& PDFDocumentBuilder::getObjectByReference(PDFObjectReference reference) const
{
    return m_storage.getObject(reference);
}

}   // namespace pdf

#endif // PDFDOCUMENTBUILDER_H