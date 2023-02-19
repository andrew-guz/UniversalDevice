#ifndef _EVENTS_TABLE_MODEL_H_
#define _EVENTS_TABLE_MODEL_H_

#include <vector>
#include <nlohmann/json.hpp>

#include <Wt/WAbstractItemModel.h>
#include <Wt/WModelIndex.h>
#include <Wt/cpp17/any.hpp>

class EventsTableModel : public Wt::WAbstractItemModel
{
public:
    EventsTableModel(const std::vector<nlohmann::json>& data = {});

    void UpdateData(const std::vector<nlohmann::json>& data);

    virtual int columnCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;

    virtual int rowCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const  override;

    virtual Wt::WModelIndex index(int row, int column, const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;

    virtual Wt::WModelIndex parent(const Wt::WModelIndex& index) const override;

    virtual Wt::cpp17::any data(const Wt::WModelIndex& index, Wt::ItemDataRole role = Wt::ItemDataRole::Display) const override;

private:
    std::vector<nlohmann::json> _data;
};

#endif //_EVENTS_TABLE_MODEL_H_
