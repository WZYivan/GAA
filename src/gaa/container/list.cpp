#include <gaa/container/list.hpp>

namespace gaa {

bool List::has_key(key_t const &k) const { return m_name.vk().contains(k); }
void List::garbage_collect() {
  Indices alive = this->valid_indices() | std::ranges::to<Indices>();
  for (Index new_i = 0uz; new_i != alive.size(); ++new_i) {
    Index old_i = alive[new_i];
    if (new_i == old_i) {
      continue;
    } else {
      m_indices[new_i] = new_i;

      m_data.insert_or_assign(new_i, m_data.at(old_i));
      m_data.erase(old_i);

      m_name.insert_or_assign(new_i, m_name.at(old_i));
      m_name.erase(old_i);
    }
  }
  m_indices.resize(alive.size());
  m_idx_pool.clear();
}

Any &List::operator[](key_t const &k) {
  if (this->has_key(k)) {
    return this->at(k);
  } else {
    this->insert_or_assign(k, nullptr);
    return this->at(k);
  }
}
Any const &List::operator[](key_t const &k) const {
  if (this->has_key(k)) {
    return this->at(k);
  } else {
    gaa_fail("key \'{}\' not exists", k);
  }
}
Any &List::operator[](Index k) { return this->operator[](this->name_of(k)); }
Any const &List::operator[](Index k) const {
  return this->operator[](this->name_of(k));
}

Any &List::at(key_t const &k) {
  Index i = m_name.at(k);
  return this->at(i);
}
Any const &List::at(key_t const &k) const {
  Index i = m_name.at(k);
  return this->at(i);
}
Any &List::at(Index i) { return m_data.at(i); }
Any const &List::at(Index i) const { return m_data.at(i); }

List::Index List::index_of(key_t const &k) const { return m_name.at(k); }
List::key_t const &List::name_of(Index idx) const { return m_name.at(idx); }

std::type_info const &List::type_info(Index i) const {
  return this->at(i).type_info();
}
std::type_info const &List::type_info(key_t const &k) const {
  return this->at(k).type_info();
}

void List::erase(Index idx) { m_idx_pool.deprecate(idx); }
void List::erase(key_t const &k) { erase(this->index_of(k)); }

std::string List::glimpse(bool with_typename) const {
  std::string cnt;
  auto out = std::back_inserter(cnt);
  for (auto const &[idx, v] : m_data) {
    if (m_idx_pool.is_deprecated(idx)) {
      std::format_to(out, "(x) ");
    }
    if (with_typename) {
      std::format_to(out, "[{}] {} <{}>: {}", idx, this->name_of(idx),
                     this->type_info(idx).name(), v.format());
    } else {
      std::format_to(out, "[{}] {} : {}", idx, this->name_of(idx), v.format());
    }

    *out = '\n';
  }
  return cnt;
}
} // namespace gaa