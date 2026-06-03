#include <gaa/container/frame.hpp>
#include <gaa/core/literal.hpp>

namespace gaa {
Frame::Frame(std::size_t size) : m_size(size) {}

std::size_t Frame::rows() const { return m_size; }
std::size_t Frame::cols() const {
  return m_indices.size() - m_idx_pool.deprecated_size();
}

Any const &Frame::col(Index i) const { return m_data.at(i); }
Any &Frame::col(Index i) { return m_data.at(i); }
Any const &Frame::col(std::string const &k) const {
  return m_data.at(this->index_of(k));
}
Any &Frame::col(std::string const &k) { return m_data.at(this->index_of(k)); }

void Frame::new_col(std::string const &k, Literal_Type literal) {
  using enum Literal_Type;
  switch (literal) {
  case Charater: {
    this->new_col<Literal_type<Charater>>(k);
    break;
  }
  case String: {
    this->new_col<Literal_type<String>>(k);
    break;
  }
  case Digital: {
    this->new_col<Literal_type<Digital>>(k);
    break;
  }
  case Integer: {
    this->new_col<Literal_type<Integer>>(k);
    break;
  }
  case Logical: {
    this->new_col<Literal_type<Logical>>(k);
    break;
  }
  case Latitude: {
    this->new_col<Literal_type<Latitude>>(k);
    break;
  }
  case Longitude: {
    this->new_col<Literal_type<Longitude>>(k);
    break;
  }
  case Radian: {
    this->new_col<Literal_type<Radian>>(k);
    break;
  }
  case Arcdeg: {
    this->new_col<Literal_type<Arcdeg>>(k);
    break;
  }
  case Arcmin: {
    this->new_col<Literal_type<Arcmin>>(k);
    break;
  }
  case Arcsec: {
    this->new_col<Literal_type<Arcsec>>(k);
    break;
  }
  default:
    gaa_fail("unreachable default case: {}", enum2str(literal));
  };
}

void Frame::garbage_collect() {
  Indices alive = this->indices() | std::ranges::to<Indices>();
  Data new_data;
  new_data.reserve(alive.size());
  for (Index new_i = 0uz; new_i != alive.size(); ++new_i) {
    Index old_i = alive[new_i];
    if (new_i == old_i) {
      new_data.emplace_back(m_data.at(old_i));
      continue;
    } else {
      m_indices[new_i] = new_i;

      new_data.emplace_back(m_data.at(old_i));

      m_name.insert_or_assign(new_i, m_name.at(old_i));
      m_name.erase(old_i);
    }
  }
  m_data.swap(new_data);
  m_indices.resize(alive.size());
  m_idx_pool.clear();
}

std::string Frame::glimpse() const {
  std::string cnt, line;
  auto out = std::back_inserter(line);

  for (auto const &[idx, any] : std::views::zip(m_indices, m_data)) {
    if (m_idx_pool.is_deprecated(idx)) {
      continue;
    }

    std::format_to(out, "[{}] {} : {}", idx, this->name_of(idx), any.format());
    if (line.size() > 96) {
      line.resize(96);
      line.append(" ...");
    }
    cnt.append(line);
    cnt.append("\n");
    line.clear();
  }

  return cnt;
}

std::string const &Frame::name_of(Index i) const { return m_name.at(i); }
Frame::Index Frame::index_of(std::string const &k) const {
  return m_name.at(k);
}

void Frame::erase_col(Index i) { m_idx_pool.deprecate(i); }
void Frame::erase_col(std::string const &k) {
  this->erase_col(this->index_of(k));
}
void Frame::rename_col(Index i, std::string const &new_name) {
  this->m_name.at(i) = new_name;
}
void Frame::rename_col(std::string const &k, std::string const &new_name) {
  this->m_name.at(this->m_name.at(k)) = new_name;
}

Frame::Row<Frame> Frame::row(Frame::Index i) {
  gaa_assert(i < this->rows(), "row {} out of range ({})", i, this->rows());
  return Row<Frame>(i, *this);
}
Frame::Row<Frame const> Frame::row(Frame::Index i) const {
  gaa_assert(i < this->rows(), "row {} out of range ({})", i, this->rows());
  return Row<Frame const>(i, *this);
}
} // namespace gaa