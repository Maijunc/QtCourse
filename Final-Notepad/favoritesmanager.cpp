#include "favoritesmanager.h"

FavoritesManager::FavoritesManager() {}

void FavoritesManager::addFavorite(const QString& path) {
    if (!favorites.contains(path)) {
        favorites.append(path);
    }
}

void FavoritesManager::removeFavorite(const QString& path) {
    favorites.removeAll(path);
}

QList<QString> FavoritesManager::getFavorites() const {
    return favorites;
}

void FavoritesManager::saveFavorites() {
    QSettings settings;
    settings.setValue(settingsKey, QVariant::fromValue(favorites));
}

void FavoritesManager::loadFavorites() {
    QSettings settings;
    favorites = settings.value(settingsKey).value<QList<QString>>();
}
