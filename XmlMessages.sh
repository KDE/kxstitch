function get_files
{
    echo kxstitch.xml
}

function po_for_file
{
    case "$1" in
       kxstitch.xml)
           echo kxstitch_xml_mimetypes.po
       ;;
    esac
}

function tags_for_file
{
    case "$1" in
       kxstitch.xml)
           echo comment
       ;;
    esac
}
