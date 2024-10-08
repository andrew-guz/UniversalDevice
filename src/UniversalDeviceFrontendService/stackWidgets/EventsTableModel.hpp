#pragma once

#include <vector>

#include <Wt/WAbstractItemModel.h>
#include <Wt/WModelIndex.h>
#include <Wt/cpp17/any.hpp>
#include <nlohmann/json.hpp>

#include "Enums.hpp"

class EventsTableModel : public Wt::WAbstractItemModel {
public:
    EventsTableModel(const std::vector<nlohmann::json>& data = {});

    void UpdateData(const std::vector<nlohmann::json>& data);

    virtual int columnCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;

    virtual int rowCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;

    virtual Wt::WModelIndex index(int row, int column, const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;

    virtual Wt::WModelIndex parent(const Wt::WModelIndex& index) const override;

    virtual Wt::cpp17::any data(const Wt::WModelIndex& index, Wt::ItemDataRole role = Wt::ItemDataRole::Display) const override;

    virtual Wt::cpp17::any headerData(int section,
                                      Wt::Orientation orientation = Wt::Orientation::Horizontal,
                                      Wt::ItemDataRole role = Wt::ItemDataRole::Display) const override;

    static std::string EventTypeDisplayName(EventType eventType);

private:
    static std::string EventAdditionalInfo(EventType eventType, const nlohmann::json& eventJson);

private:
    std::vector<nlohmann::json> _data;
};
