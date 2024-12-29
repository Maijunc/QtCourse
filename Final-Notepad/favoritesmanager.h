#ifndef FAVORITESMANAGER_H
#define FAVORITESMANAGER_H

#include <QList>
#include <QString>
#include <QSettings>

class FavoritesManager {
public:
    void addFavorite(const QString& path);
    void removeFavorite(const QString& path);
    QList<QString> getFavorites() const;
    void saveFavorites();
    void loadFavorites();

    FavoritesManager();
private:
    QList<QString> favorites;
    const QString settingsKey = "Favorites";
};


#endif // FAVORITESMANAGER_H
