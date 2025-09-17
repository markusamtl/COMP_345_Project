    #ifndef MAPLOADER_H
    #define MAPLOADER_H

    #include <string>
    #include <map>
    #include <vector>

    using namespace std;

    class MapLoader {

    private:             

        //Metadata
        string author;
        string image;
        string wrap;
        string scrollType;
        string warn;
        
        //Continent Info
        map<string, int> continent; 
        
        //Territory Info
        vector<vector<string>> territories;
        

    public:
        
        //-- Constructors, Destructor, Copy Constructor, Assignment Operator, Stream Insertion Operator  --//

        MapLoader();

        MapLoader(string author, string image, string wrap, string scrollType, string warn, map<string, int> continent, vector<vector<string>> territories);

        ~MapLoader();

        MapLoader(const MapLoader& other);
        
        MapLoader& operator=(const MapLoader& other);
        
        friend ostream& operator<<(ostream& os, const MapLoader& mapLoader);

        //-- Accessors and Mutators --//

        const string& getAuthor() const;
        void setAuthor(string& author);
        
        const string& getImage() const;
        void setImage(string& image);
        
        const string& getWrap() const;
        void setWrap(string& wrap);

        const string& getScrollType() const;
        void setScrollType(string& scrollType);

        const string& getWarn() const;
        void setWarn(string& warn);

        const map<string, int>& getContinent() const;
        void setContinent(map<string, int> continent);

        const vector<vector<string>>& getTerritories() const;
        void setTerritories(vector<vector<string>> territories);
        
        //-- Class Methods --//

        string trim(const string& s);

        int importMapInfo(const string& filePath);

    };

    #endif