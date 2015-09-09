#ifndef IPV4ALIDATOR_H
#define IPV4ALIDATOR_H
class IPV4Validator : public QValidator {
    public:
        IPV4Validator(QObject *parent=0) : QValidator(parent){}
        State validate ( QString & input, int & pos ) const {
        if(input.isEmpty())
            return Intermediate;
        QStringList sections = input.split(".");
        if(sections.count()>4)
            return Invalid;
        int newPos = pos;
        int grp = 0;
        bool isEmpty = false;
        while(grp<sections.count()){
            QString txt = sections[grp];
            if(txt.count()>3){
                if(sections.count()==4)
                    return Invalid;
                sections[grp] = txt.left(3);
                sections.insert(grp+1, txt.mid(3));
                newPos++;
                }
            else if(txt.count()==0){
                isEmpty = true;
                grp++;
                continue;
            }
        txt = sections[grp];
        if(txt[0]=='0' && txt[1]=='0'){
            sections[grp] = '0';
            if(sections.count()<4){
                sections.insert(grp+1, sections[grp]);
                newPos++;
            }
            }
            else if(txt[0]=='0' && txt.count()>1){
                sections[grp] = txt.mid(1);
                newPos--;
            }
            bool ok;
            int val = txt.toInt(&ok);
        if(!ok)
        {
            if(grp < 2)
            return Invalid;
            if(txt != "*")
                return Invalid;
        }
        if(val>255 && sections.count()<4){
            sections[grp] = txt.left(2);
            sections.insert(grp+1, txt.mid(2));
            val = txt.left(2).toInt();
            newPos++;
        }
        if(val>255||val<0) return Invalid;
            grp++;
        }
        input = sections.join(".");
        // if(newPos==input.count() && sections.count()<4 && input[input.count()-1]!='.' && sections.last().count()==3){
        // newPos++;
        // input += ".";
        // }
        pos = newPos;
        if(sections.count()==4 && !isEmpty) return Acceptable;
        return Intermediate;
        }
};
#endif // IPV4ALIDATOR_H
